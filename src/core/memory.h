#pragma once

#ifndef POL_L1_CACHE_LINE_SIZE
#define POL_L1_CACHE_LINE_SIZE 64
#endif

namespace pol {
	void* AllocAligned(int size);
	void FreeAligned(void* p);
	template<class T>
	T* AllocAligned(int count) {
		return (T*)AllocAligned(count * sizeof(T));
	}

	//In C++, 2D arrays are arranged in memory so that entire rows of values are contiguous
	//in memory. This is not always an optimal layout, however; fo such an array indexed by(u,v),
	//nearby(u,v) array position will often map to distant memory locations. For all but the
	//smallest arrays, the adjacent values in the v direction will be on different cache lines;
	//thus, if the cost of a cache miss is incurred to reference a value at a particular location(u,v)
	//there is no chance that handling that miss will allso load into memory the data for values
	//(u, v+1), (u, v-1), and so on. Thus, spatially coherent array indices in (u,v) do not necessarily
	//lead to the spatially coherent memory access patterns that modern memory caches depend on.
	//To address this problem, the BlockedArray template implements a generic 2D array of values,
	//with the items ordered in memory using a blocked memory layout. The array is subdivided into
	//square blocks of a small fixed size that is a power of 2. Each block is laid out row by row,
	//as if th were a separate 2D c++ array. This organization substantially improves the memory coherence
	//of 2D array references in practice and requires only a small amount of additional computation to 
	//determine the memory address for a particular position
	//To ensure that the block size is a power of 2, the caller specufies its logarithm(base 2),
	//which is given by the template parameter logBlockSize
	//
	//CODE FROM PBRT
	template <class T, int logBlockSize>
	class BlockedArray {
	private:
		T* data;
		const int uRes, vRes, uBlocks;

	public:
		BlockedArray(int uRes, int vRes, const T* d = nullptr)
			:uRes(uRes), vRes(vRes), uBlocks(RoundUp(uRes) >> logBlockSize) {
			int nAlloc = RoundUp(uRes) * RoundUp(vRes);
			data = AllocAligned<T>(nAlloc);
			for (int i = 0; i < nAlloc; ++i)
				new (&data[i])T(); //execute construct method
			if (d) {
				for (int v = 0; v < vRes; ++v) {
					for (int u = 0; u < uRes; ++u) {
						(*this)(u, v) = d[v*uRes + u];
					}
				}
			}
		}

		~BlockedArray() {
			for (int i = 0; i < RoundUp(uRes) * RoundUp(vRes); ++i)
				data[i].~T();
			FreeAligned(data);
		}

		int BlockSize() const {
			return 1 << logBlockSize;
		}

		int RoundUp(int x) const {
			return (x + BlockSize() - 1) & ~(BlockSize() - 1);
		}

		int uSize() const {
			return uRes;
		}

		int vSize() const {
			return vRes;
		}

		int Block(int a) const {
			return a >> logBlockSize;
		}

		int Offset(int a) const {
			return (a & (BlockSize() - 1));
		}

		T& operator()(int u, int v) {
			int bu = Block(u), bv = Block(v);
			int ou = Offset(u), ov = Offset(v);
			int offset = BlockSize() * BlockSize() * (uBlocks * bv + bu);
			offset += BlockSize() * ov + ou;
			return data[offset];
		}

		const T& operator()(int u, int v) const {
			int bu = Block(u), bv = Block(v);
			int ou = Offset(u), ov = Offset(v);
			int offset = BlockSize() * BlockSize() * (uBlocks * bv + bu);
			offset += BlockSize() * ov + ou;
			return data[offset];
		}
	};
}