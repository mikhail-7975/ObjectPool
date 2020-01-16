#pragma once
#include <new>
#include <vector>

template< class T>
class ObjectPool final {
	std::vector<bool> _elementStateVector;
	T* pool;
	size_t _poolSize;
	size_t _usedElementsCount;
public:
	ObjectPool()=delete;
	ObjectPool(size_t maxSize) {
		pool = static_cast<T*>(operator new[](maxSize*sizeof(T)));
		_poolSize = maxSize;
		_usedElementsCount = 0;
		_elementStateVector.resize(maxSize, false);
		
	};
	~ObjectPool() {
		for (size_t i = 0; i<_poolSize; i++) {
			free(pool + i);
		}
		operator delete (pool);
	};

	template< class... Arguments>
	decltype(auto)  alloc(Arguments&& ... args) {

		for (size_t i = 0; i < _poolSize;i++) {
			if (!_elementStateVector[i]) {
				_elementStateVector[i] = true;
				_usedElementsCount++;
				return new(pool+i)T(std::forward<Arguments>(args)...);
			}
		}
		throw std::length_error("can't add new element");
	};

	void free(T* object) {
		size_t number = object - pool;
		if (number < _poolSize&& _elementStateVector[number])
		{
			_elementStateVector[number] = false;
			_usedElementsCount--;
			(pool+number)->~T();
		}
	};

	size_t usedElementsCount() const {
		return _usedElementsCount;
	};
};