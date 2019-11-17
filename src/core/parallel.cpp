#include "parallel.h"
#include <queue>
#include <atomic>

//parallel notes from pbrt
//Cache coherence is a feature of all modern multicore CPUs; with it, memory writes by
//one processor are automatically viisible to other processors. This is an incredibly useful
//feature; being able to assume it in the implementation of a syatem like pbrt is extremely
//helpful to the programmer. Understanding the stubtleties and performance characteristics
//of this feature is important, however.
//
//One potential issue is that other processors may not see writes to memory in the same
//order that the processor that performed the writes issued them. This can happen for two
//main reasons:the compiler's optimizer may have reordered write operations to improve 
//performance, and the CPU hardware may write values to memory in a different order
//than the stream of executed machine instructions. In the single-threaded case, both of 
//these are innocuous; by design, the compiler and hardware, respectively, ensure that
//it's impossible for a single thread of execution running the program to detect when 
//these cases happen. This guarantee is not provided for multi-threaded code. however;
//doing so would impose a significant performance penalty,so hardware architectures leave
//handling this problem, when it matters, to software.
//
//Memory barrier instructions can be used to ensure that all write instructions before the
//barrier are visible in memory before any subsequent instructions execute.In practice,
//we generally don't need to issue memory barrier instructions explicitly, since the thread
//synchronization calls used to build multi-threaded algorithms take care of this; they
//are defined to make sure that writes are visible so that if we aree coordinating execution
//between multiple threads using these calls, then they have a consistent view of memory 
//after synchronization points.
//
//Although cache coherence is helpful to programmer, it can sometimes impose a
//substantial performance penalty for data that is frequently modified and accessed by
//multiple processors. Read-only data has little penalty; copies of it can be stored in the
//local caches of all of the processors that are accessing it, allowing all of them the same
//performace benefits from the caches as in the single-threaded case.To understand the
//downside of taking too much advantage of cache coherence for read-write data, it's useful
//to understand how cache coherence is typically implemented on processors
//
//CPUs implement a cache coherence protocol, which is responsible for tracking the memory
//transactions issued by all of the processors in order to provide cache coherence. A classic
//such protocol is MESI, where the acronym represents the four states that each cache line
//can be in. Each processor stores the current state for each cache line in its local caches:
//Modified--The current processor has written to the memory location,but the result is only
//   stored in the cache -- it's dirty and hasn\t been writeen to main memory. No other processor 
//   has the localtion in its cache.
//Exclusive--The current processor is the only one with the data from the corresponding memory 
//   location in its cache. The value in the cache matches the value in memory.
//Shared--Multiple processors have the corresponding memory location in their caches, but they 
//   have only performed read operations.
//Invalie--The cache line doesn't hold valid data.
//At system startup time, the caches are empty and all cache lines are in the invalid state.
//The first time a processor reads a memory location, the data for that location is loaded
//into cache and its cache line marked as being in the "exclusive" state. If another processor
//performs a memory read of a location that is in the "exclusive" state in another cache,
//then both caches record the state for the corresponding memory location to instead be "shared".
//
//When a processor writes to a memory location, the performance of the write depends on the state
//of the corresponding cache line. If th's in the "exclusive" state and already in the writing 
//processor's cache, then the write is cheap;the data is modified in the cache and the cache line's
//state is changed to "modified".(If it was already in the "modified" state, then the write is 
//similarly efficient). In these acses, the value will eventually be written to main memory, at
//which point the corresponding cache line returns to the "exclusive" state.
//
//However, if a processor writes to a memory location that's in the "shared" state in its cache or
//is in the "modified" or "exclusive" state in another processor's cache, then expensive communication
//between the cores is required.All of this is handled transparently by the hardware, though it is still
//has a performance impact. In this case, the writing processor must issue a read for ownership(RFO),
//which marks the memory location as invalid in the caches of any  other processors;RFOs can cause 
//stalls of tens or hundreds of cycles--a substantial penalty for a single memory write.
//
//In general, we'd therefore like to avoid the situation of multiple processors concirrently writing
//to the same memory location as well as unnecessarily reading memory that another processor is 
//writing to.An important case to be aware of is "false sharing" where a single cache line holds some 
//read-only data and some data that is frequently modified. In this case, even if only a single processor
//is writing to the part of the cache line that is modified  but many are reading from the read-only part,
//the overhead of frequent RFO operations will be unnecessarily incurred.
//
//A situation where many processors might be concurrently trying to write to the same or neaby memory
//locations is when image sample values are accumulated into the final image. To ensure that image updates
//don't pay the RFO cost each rendering thread in the ParallelFor() loop of the SamplerIntegrator creates a 
//private FilmTile to use for accumulating sample values for the part of the image that it's working on; it
//is then free to modifiy the FilmTile pixel values without worrying about contention with other thread for 
//those memory locations. Only when a portion of the image is finished is the tile merged into the main image
//thus allowing the overhead of mutual exclusion and RFO operations to be amortized over a smaller number of 
//larger updates.

namespace pol {
	vector<thread*> Parallel::threads;
	atomic<int> activeThreads = 0;
	mutex taskBarrier, reportBarrier;
	int nTasks;
	atomic<int> nextTaskId = 0;
	queue<RenderBlock> tasks;
	function<void(const RenderBlock & rb)> func;
	void ThreadEntry(int index) {
		while (true) {
			taskBarrier.lock();
			if (tasks.empty()) {
				taskBarrier.unlock();
				continue;
			}
			const RenderBlock& rb = tasks.front();
			tasks.pop();
			taskBarrier.unlock();
			activeThreads++;

			func(rb);

			activeThreads--;
			nextTaskId++;
			reportBarrier.lock();
			printf("Rendering Progress[%.3f%%]\r", Float(nTasks - 1 - tasks.size()) / (nTasks - 1) * 100);
			reportBarrier.unlock();
		}

	}

	void Parallel::Startup() {
		int nCores = GetNumSystemCores();
		threads.resize(nCores);
		for (int i = 0; i < nCores; ++i) {
			threads[i] = new thread(ThreadEntry, i);
			threads[i]->detach();
		}
	}

	void Parallel::Shutdown() {
		for (thread* t : threads) {
			delete t;
		}
	}

	void Parallel::ParallelLoop(function<void(const RenderBlock & rb)> f, const vector<RenderBlock>& rbs) {
		taskBarrier.lock();
		nTasks = rbs.size();
		for (int i = 0; i < nTasks; ++i) {
			tasks.push(rbs[i]);
		}
		func = f;
		taskBarrier.unlock();
	}

	bool Parallel::IsFinish() {
		return nextTaskId >= nTasks && activeThreads == 0;
	}

	int Parallel::GetNumSystemCores() {
		return thread::hardware_concurrency();
	}
}