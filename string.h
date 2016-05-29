#ifndef HIDER_FILE_TY_STRING
#define HIDER_FILE_TY_STRING

#include "Allocator.h"
#include "Types.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

namespace _core
{
	enum eLocaleID
	{
		CORE_LOCALE_ANSI = 0,
		CORE_LOCALE_GERMAN = 1
	};

	static eLocaleID locale_current = CORE_LOCALE_ANSI;
	static inline void locale_set ( eLocaleID id )
	{
		locale_current = id;
	}

	static inline u32 locale_lower ( u32 x )
	{
		switch ( locale_current )
		{
			case CORE_LOCALE_GERMAN:
			case CORE_LOCALE_ANSI:
				break;
		}
		return x >= 'A' && x <= 'Z' ? x + 0x20 : x;
	}

	static inline u32 locale_upper ( u32 x )
	{
		switch ( locale_current )
		{
			case CORE_LOCALE_GERMAN:
			case CORE_LOCALE_ANSI:
				break;
		}

		return x >= 'a' && x <= 'z' ? x + ( 'A' - 'a' ) : x;
	}


	template <typename T, typename TAlloc = irrAllocator<T> >
	class string
	{
	public:
		typedef T char_type;

		string() : array(0), allocated(1), used(1)
		{
			array = allocator.allocate(1);
			array[0] = 0;
		}

		string(const string<T,TAlloc>& other) : array(0), allocated(0), used(0)
		{
			*this = other;
		}

		template <class B, class A> string(const string<B, A>& other) : array(0), allocated(0), used(0)
		{
			*this = other;
		}

		explicit string(const double number) : array(0), allocated(0), used(0)
		{
			c8 tmpbuf[255];
			sprintf_s(tmpbuf, 255, "%0.6f", number);
			*this = tmpbuf;
		}

		explicit string(int number) : array(0), allocated(0), used(0)
		{
			bool negative = false;
			if (number < 0)
			{
				number *= -1;
				negative = true;
			}

			c8 tmpbuf[16]={0};
			u32 idx = 15;

			if (!number)
			{
				tmpbuf[14] = '0';
				*this = &tmpbuf[14];
				return;
			}

			while(number && idx)
			{
				--idx;
				tmpbuf[idx] = (c8)('0' + (number % 10));
				number /= 10;
			}

			if (negative)
			{
				--idx;
				tmpbuf[idx] = '-';
			}

			*this = &tmpbuf[idx];
		}


		explicit string(unsigned int number) : array(0), allocated(0), used(0)
		{
			c8 tmpbuf[16]={0};
			u32 idx = 15;

			if (!number)
			{
				tmpbuf[14] = '0';
				*this = &tmpbuf[14];
				return;
			}

			while(number && idx)
			{
				--idx;
				tmpbuf[idx] = (c8)('0' + (number % 10));
				number /= 10;
			}

			*this = &tmpbuf[idx];
		}


		explicit string(long number) : array(0), allocated(0), used(0)
		{
			bool negative = false;
			if (number < 0)
			{
				number *= -1;
				negative = true;
			}
			c8 tmpbuf[16]={0};
			u32 idx = 15;

			if (!number)
			{
				tmpbuf[14] = '0';
				*this = &tmpbuf[14];
				return;
			}

			while(number && idx)
			{
				--idx;
				tmpbuf[idx] = (c8)('0' + (number % 10));
				number /= 10;
			}

			if (negative)
			{
				--idx;
				tmpbuf[idx] = '-';
			}

			*this = &tmpbuf[idx];
		}


		explicit string(unsigned long number) : array(0), allocated(0), used(0)
		{
			c8 tmpbuf[16]={0};
			u32 idx = 15;

			if (!number)
			{
				tmpbuf[14] = '0';
				*this = &tmpbuf[14];
				return;
			}

			while(number && idx)
			{
				--idx;
				tmpbuf[idx] = (c8)('0' + (number % 10));
				number /= 10;
			}

			*this = &tmpbuf[idx];
		}


		template <class B>
		string(const B* const c, u32 length) : array(0), allocated(0), used(0)
		{
			if (!c)
			{
				*this="";
				return;
			}

			allocated = used = length+1;
			array = allocator.allocate(used);

			for (u32 l = 0; l<length; ++l)
				array[l] = (T)c[l];

			array[length] = 0;
		}


		template <class B>
		string(const B* const c) : array(0), allocated(0), used(0)
		{
			*this = c;
		}

		~string()
		{
			allocator.deallocate(array);
		}


		string<T,TAlloc>& operator=(const string<T,TAlloc>& other)
		{
			if (this == &other)
				return *this;

			used = other.size()+1;
			if (used>allocated)
			{
				allocator.deallocate(array);
				allocated = used;
				array = allocator.allocate(used);
			}

			const T* p = other.c_str();
			for (u32 i=0; i<used; ++i, ++p)
				array[i] = *p;

			return *this;
		}

		template <class B, class A>
		string<T,TAlloc>& operator=(const string<B,A>& other)
		{
			*this = other.c_str();
			return *this;
		}


		template <class B>
		string<T,TAlloc>& operator=(const B* const c)
		{
			if (!c)
			{
				if (!array)
				{
					array = allocator.allocate(1);
					allocated = 1;
				}
				used = 1;
				array[0] = 0x0;
				return *this;
			}

			if ((void*)c == (void*)array)
				return *this;

			u32 len = 0;
			const B* p = c;
			do
			{
				++len;
			} while(*p++);

			T* oldArray = array;

			used = len;
			if (used>allocated)
			{
				allocated = used;
				array = allocator.allocate(used);
			}

			for (u32 l = 0; l<len; ++l)
				array[l] = (T)c[l];

			if (oldArray != array)
				allocator.deallocate(oldArray);

			return *this;
		}


		string<T,TAlloc> operator+(const string<T,TAlloc>& other) const
		{
			string<T,TAlloc> str(*this);
			str.append(other);

			return str;
		}


		template <class B>
		string<T,TAlloc> operator+(const B* const c) const
		{
			string<T,TAlloc> str(*this);
			str.append(c);

			return str;
		}


		T& operator [](const u32 index)
		{
			_DEBUG_BREAK_IF(index>=used)
			return array[index];
		}


		const T& operator [](const u32 index) const
		{
			_DEBUG_BREAK_IF(index>=used)
			return array[index];
		}


		bool operator==(const T* const str) const
		{
			if (!str)
				return false;

			u32 i;
			for (i=0; array[i] && str[i]; ++i)
				if (array[i] != str[i])
					return false;

			return (!array[i] && !str[i]);
		}


		bool operator==(const string<T,TAlloc>& other) const
		{
			for (u32 i=0; array[i] && other.array[i]; ++i)
				if (array[i] != other.array[i])
					return false;

			return used == other.used;
		}


		bool operator<(const string<T,TAlloc>& other) const
		{
			for (u32 i=0; array[i] && other.array[i]; ++i)
			{
				const s32 diff = array[i] - other.array[i];
				if (diff)
					return (diff < 0);
			}

			return (used < other.used);
		}


		bool operator!=(const T* const str) const
		{
			return !(*this == str);
		}


		bool operator!=(const string<T,TAlloc>& other) const
		{
			return !(*this == other);
		}

		u32 size() const
		{
			return used-1;
		}

		bool empty() const
		{
			return (size() == 0);
		}

		const T* c_str() const
		{
			return array;
		}

		string<T,TAlloc>& make_lower()
		{
			for (u32 i=0; array[i]; ++i)
				array[i] = locale_lower ( array[i] );
			return *this;
		}

		string<T,TAlloc>& make_upper()
		{
			for (u32 i=0; array[i]; ++i)
				array[i] = locale_upper ( array[i] );
			return *this;
		}

		bool equals_ignore_case(const string<T,TAlloc>& other) const
		{
			for(u32 i=0; array[i] && other[i]; ++i)
				if (locale_lower( array[i]) != locale_lower(other[i]))
					return false;

			return used == other.used;
		}

		bool equals_substring_ignore_case(const string<T,TAlloc>&other, const s32 sourcePos = 0 ) const
		{
			if ( (u32) sourcePos >= used )
				return false;

			u32 i;
			for( i=0; array[sourcePos + i] && other[i]; ++i)
				if (locale_lower( array[sourcePos + i]) != locale_lower(other[i]))
					return false;

			return array[sourcePos + i] == 0 && other[i] == 0;
		}

		bool lower_ignore_case(const string<T,TAlloc>& other) const
		{
			for(u32 i=0; array[i] && other.array[i]; ++i)
			{
				s32 diff = (s32) locale_lower ( array[i] ) - (s32) locale_lower ( other.array[i] );
				if ( diff )
					return diff < 0;
			}

			return used < other.used;
		}

		bool equalsn(const string<T,TAlloc>& other, u32 n) const
		{
			u32 i;
			for(i=0; array[i] && other[i] && i < n; ++i)
				if (array[i] != other[i])
					return false;

			return (i == n) || (used == other.used);
		}


		bool equalsn(const T* const str, u32 n) const
		{
			if (!str)
				return false;
			u32 i;
			for(i=0; array[i] && str[i] && i < n; ++i)
				if (array[i] != str[i])
					return false;

			return (i == n) || (array[i] == 0 && str[i] == 0);
		}


		string<T,TAlloc>& append(T character)
		{
			if (used + 1 > allocated)
				reallocate(used + 1);

			++used;

			array[used-2] = character;
			array[used-1] = 0;

			return *this;
		}

		string<T,TAlloc>& append(const T* const other, u32 length=0xffffffff)
		{
			if (!other)
				return *this;

			u32 len = 0;
			const T* p = other;
			while(*p)
			{
				++len;
				++p;
			}
			if (len > length)
				len = length;

			if (used + len > allocated)
				reallocate(used + len);

			--used;
			++len;

			for (u32 l=0; l<len; ++l)
				array[l+used] = *(other+l);

			used += len;

			return *this;
		}

		string<T,TAlloc>& append(const string<T,TAlloc>& other)
		{
			if (other.size() == 0)
				return *this;

			--used;
			u32 len = other.size()+1;

			if (used + len > allocated)
				reallocate(used + len);

			for (u32 l=0; l<len; ++l)
				array[used+l] = other[l];

			used += len;

			return *this;
		}


		string<T,TAlloc>& append(const string<T,TAlloc>& other, u32 length)
		{
			if (other.size() == 0)
				return *this;

			if (other.size() < length)
			{
				append(other);
				return *this;
			}

			if (used + length > allocated)
				reallocate(used + length);

			--used;

			for (u32 l=0; l<length; ++l)
				array[l+used] = other[l];
			used += length;

			array[used]=0;
			++used;

			return *this;
		}

		void reserve(u32 count)
		{
			if (count < allocated)
				return;

			reallocate(count);
		}


		s32 findFirst(T c) const
		{
			for (u32 i=0; i<used-1; ++i)
				if (array[i] == c)
					return i;

			return -1;
		}

		s32 findFirstChar(const T* const c, u32 count=1) const
		{
			if (!c || !count)
				return -1;

			for (u32 i=0; i<used-1; ++i)
				for (u32 j=0; j<count; ++j)
					if (array[i] == c[j])
						return i;

			return -1;
		}

		template <class B>
		s32 findFirstCharNotInList(const B* const c, u32 count=1) const
		{
			if (!c || !count)
				return -1;

			for (u32 i=0; i<used-1; ++i)
			{
				u32 j;
				for (j=0; j<count; ++j)
					if (array[i] == c[j])
						break;

				if (j==count)
					return i;
			}

			return -1;
		}

		template <class B>
		s32 findLastCharNotInList(const B* const c, u32 count=1) const
		{
			if (!c || !count)
				return -1;

			for (s32 i=(s32)(used-2); i>=0; --i)
			{
				u32 j;
				for (j=0; j<count; ++j)
					if (array[i] == c[j])
						break;

				if (j==count)
					return i;
			}

			return -1;
		}

		s32 findNext(T c, u32 startPos) const
		{
			for (u32 i=startPos; i<used-1; ++i)
				if (array[i] == c)
					return i;

			return -1;
		}


		s32 findLast(T c, s32 start = -1) const
		{
			start = core::clamp ( start < 0 ? (s32)(used) - 2 : start, 0, (s32)(used) - 2 );
			for (s32 i=start; i>=0; --i)
				if (array[i] == c)
					return i;

			return -1;
		}

		s32 findLastChar(const T* const c, u32 count=1) const
		{
			if (!c || !count)
				return -1;

			for (s32 i=(s32)used-2; i>=0; --i)
				for (u32 j=0; j<count; ++j)
					if (array[i] == c[j])
						return i;

			return -1;
		}

		template <class B>
		s32 find(const B* const str, const u32 start = 0) const
		{
			if (str && *str)
			{
				u32 len = 0;

				while (str[len])
					++len;

				if (len > used-1)
					return -1;

				for (u32 i=start; i<used-len; ++i)
				{
					u32 j=0;

					while(str[j] && array[i+j] == str[j])
						++j;

					if (!str[j])
						return i;
				}
			}

			return -1;
		}

		string<T> subString(u32 begin, s32 length, bool make_lower = false ) const
		{
			if ((length <= 0) || (begin>=size()))
				return string<T>("");
			if ((length+begin) > size())
				length = size()-begin;

			string<T> o;
			o.reserve(length+1);

			s32 i;
			if ( !make_lower )
			{
				for (i=0; i<length; ++i)
					o.array[i] = array[i+begin];
			}
			else
			{
				for (i=0; i<length; ++i)
					o.array[i] = locale_lower ( array[i+begin] );
			}

			o.array[length] = 0;
			o.used = length + 1;

			return o;
		}


		string<T,TAlloc>& operator += (T c)
		{
			append(c);
			return *this;
		}


		string<T,TAlloc>& operator += (const T* const c)
		{
			append(c);
			return *this;
		}


		string<T,TAlloc>& operator += (const string<T,TAlloc>& other)
		{
			append(other);
			return *this;
		}


		string<T,TAlloc>& operator += (const int i)
		{
			append(string<T,TAlloc>(i));
			return *this;
		}

		string<T,TAlloc>& operator += (const unsigned int i)
		{
			append(string<T,TAlloc>(i));
			return *this;
		}


		string<T,TAlloc>& operator += (const long i)
		{
			append(string<T,TAlloc>(i));
			return *this;
		}


		string<T,TAlloc>& operator += (const unsigned long i)
		{
			append(string<T,TAlloc>(i));
			return *this;
		}

		string<T,TAlloc>& operator += (const double i)
		{
			append(string<T,TAlloc>(i));
			return *this;
		}

		string<T,TAlloc>& operator += (const float i)
		{
			append(string<T,TAlloc>(i));
			return *this;
		}

		string<T,TAlloc>& replace(T toReplace, T replaceWith)
		{
			for (u32 i=0; i<used-1; ++i)
				if (array[i] == toReplace)
					array[i] = replaceWith;
			return *this;
		}

		string<T,TAlloc>& replace(const string<T,TAlloc>& toReplace, const string<T,TAlloc>& replaceWith)
		{
			if (toReplace.size() == 0)
				return *this;

			const T* other = toReplace.c_str();
			const T* replace = replaceWith.c_str();
			const u32 other_size = toReplace.size();
			const u32 replace_size = replaceWith.size();

			s32 delta = replace_size - other_size;

			if (delta == 0)
			{
				s32 pos = 0;
				while ((pos = find(other, pos)) != -1)
				{
					for (u32 i = 0; i < replace_size; ++i)
						array[pos + i] = replace[i];
					++pos;
				}
				return *this;
			}

			if (delta < 0)
			{
				u32 i = 0;
				for (u32 pos = 0; pos < used; ++i, ++pos)
				{
					if (array[pos] == *other)
					{
						u32 j;
						for (j = 0; j < other_size; ++j)
						{
							if (array[pos + j] != other[j])
								break;
						}

						if (j == other_size)
						{
							for (j = 0; j < replace_size; ++j)
								array[i + j] = replace[j];
							i += replace_size - 1;
							pos += other_size - 1;
							continue;
						}
					}

					array[i] = array[pos];
				}
				array[i-1] = 0;
				used = i;

				return *this;
			}

			u32 find_count = 0;
			s32 pos = 0;
			while ((pos = find(other, pos)) != -1)
			{
				++find_count;
				++pos;
			}

			u32 len = delta * find_count;
			if (used + len > allocated)
				reallocate(used + len);

			pos = 0;
			while ((pos = find(other, pos)) != -1)
			{
				T* start = array + pos + other_size - 1;
				T* ptr   = array + used - 1;
				T* end   = array + delta + used -1;

				while (ptr != start)
				{
					*end = *ptr;
					--ptr;
					--end;
				}

				for (u32 i = 0; i < replace_size; ++i)
					array[pos + i] = replace[i];

				pos += replace_size;
				used += delta;
			}

			return *this;
		}


		string<T,TAlloc>& remove(T c)
		{
			u32 pos = 0;
			u32 found = 0;
			for (u32 i=0; i<used-1; ++i)
			{
				if (array[i] == c)
				{
					++found;
					continue;
				}

				array[pos++] = array[i];
			}
			used -= found;
			array[used-1] = 0;
			return *this;
		}

		string<T,TAlloc>& remove(const string<T,TAlloc>& toRemove)
		{
			u32 size = toRemove.size();
			if ( size == 0 )
				return *this;
			u32 pos = 0;
			u32 found = 0;
			for (u32 i=0; i<used-1; ++i)
			{
				u32 j = 0;
				while (j < size)
				{
					if (array[i + j] != toRemove[j])
						break;
					++j;
				}
				if (j == size)
				{
					found += size;
					i += size - 1;
					continue;
				}

				array[pos++] = array[i];
			}
			used -= found;
			array[used-1] = 0;
			return *this;
		}

		string<T,TAlloc>& removeChars(const string<T,TAlloc> & characters)
		{
			if (characters.size() == 0)
				return *this;

			u32 pos = 0;
			u32 found = 0;
			for (u32 i=0; i<used-1; ++i)
			{
				bool docontinue = false;
				for (u32 j=0; j<characters.size(); ++j)
				{
					if (characters[j] == array[i])
					{
						++found;
						docontinue = true;
						break;
					}
				}
				if (docontinue)
					continue;

				array[pos++] = array[i];
			}
			used -= found;
			array[used-1] = 0;

			return *this;
		}

		string<T,TAlloc>& trim(const string<T,TAlloc> & whitespace = " \t\n\r")
		{
			const s32 begin = findFirstCharNotInList(whitespace.c_str(), whitespace.used);
			if (begin == -1)
				return (*this="");

			const s32 end = findLastCharNotInList(whitespace.c_str(), whitespace.used);

			return (*this = subString(begin, (end +1) - begin));
		}

		string<T,TAlloc>& erase(u32 index)
		{
			_IRR_DEBUG_BREAK_IF(index>=used)

			for (u32 i=index+1; i<used; ++i)
				array[i-1] = array[i];

			--used;
			return *this;
		}

		string<T,TAlloc>& validate()
		{
			for (u32 i=0; i<allocated; ++i)
			{
				if (array[i] == 0)
				{
					used = i + 1;
					return *this;
				}
			}

			if ( allocated > 0 )
			{
				used = allocated;
				array[used-1] = 0;
			}
			else
			{
				used = 0;
			}

			return *this;
		}

		T lastChar() const
		{
			return used > 1 ? array[used-2] : 0;
		}

		template<class container>
		u32 split(container& ret, const T* const c, u32 count=1, bool ignoreEmptyTokens=true, bool keepSeparators=false) const
		{
			if (!c)
				return 0;

			const u32 oldSize=ret.size();
			u32 lastpos = 0;
			bool lastWasSeparator = false;
			for (u32 i=0; i<used; ++i)
			{
				bool foundSeparator = false;
				for (u32 j=0; j<count; ++j)
				{
					if (array[i] == c[j])
					{
						if ((!ignoreEmptyTokens || i - lastpos != 0) &&
								!lastWasSeparator)
							ret.push_back(string<T,TAlloc>(&array[lastpos], i - lastpos));
						foundSeparator = true;
						lastpos = (keepSeparators ? i : i + 1);
						break;
					}
				}
				lastWasSeparator = foundSeparator;
			}
			if ((used - 1) > lastpos)
				ret.push_back(string<T,TAlloc>(&array[lastpos], (used - 1) - lastpos));
			return ret.size()-oldSize;
		}

	private:

		void reallocate(u32 new_size)
		{
			T* old_array = array;

			array = allocator.allocate(new_size); //new T[new_size];
			allocated = new_size;

			u32 amount = used < new_size ? used : new_size;
			for (u32 i=0; i<amount; ++i)
				array[i] = old_array[i];

			if (allocated < used)
				used = allocated;

			allocator.deallocate(old_array);
		}

		T* array;
		u32 allocated;
		u32 used;
		TAlloc allocator;
	};

	typedef string<c8> stringc;
	typedef string<wchar_t> stringw;

}
#endif // !HIDER_FILE_TY_STRING
