#ifndef HIDER_FILE_ALLOCATOR
#define HIDER_FILE_ALLOCATOR

namespace _core
{
	template<typename T>
	class irrAllocator
	{
	public:
		virtual ~irrAllocator() {}

		T* allocate(size_t cnt)
		{
			return (T*)internal_new(cnt* sizeof(T));
		}

		void deallocate(T* ptr)
		{
			internal_delete(ptr);
		}

		void construct(T* ptr, const T&e)
		{
			new ((void*)ptr) T(e);
		}

		void destruct(T* ptr)
		{
			ptr->~T();
		}

	protected:

		virtual void* internal_new(size_t cnt)
		{
			return operator new(cnt);
		}

		virtual void internal_delete(void* ptr)
		{
			operator delete(ptr);
		}

	};

};
#endif // !HIDER_FILE_ALLOCATOR


