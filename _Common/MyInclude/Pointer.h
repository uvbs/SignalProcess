
#pragma once

///////////////////////////////////////////////////////////////////////////
// TArrayPointer
///////////////////////////////////////////////////////////////////////////
template<typename ELEMENTTYPE> class TArrayPointer
{
public:
	TArrayPointer(ELEMENTTYPE* pointer, DWORD count) : _pointer(pointer),_count(count){}
	~TArrayPointer()
	{
		if(_pointer) delete []_pointer;
		_pointer = NULL;
		_count = 0;
	}

	TArrayPointer(TArrayPointer& v):_pointer(v._pointer),_count(v._count)
	{
		v.Detach();
	}
	TArrayPointer& operator = (TArrayPointer& v)
	{
		_count = v._count;
		_pointer = v.Detach();
		return *this;
	}

	bool IsEmpty() const{return (_pointer == NULL) || (_count == 0);}

public:
	const ELEMENTTYPE* Pointer() const{return _pointer;}
	ELEMENTTYPE* Pointer(){return _pointer;}

	DWORD Count() const{return _count;}

	const ELEMENTTYPE* operator()(DWORD index) const
	{
		return index < _count ? &_pointer[index] : NULL;
	}
	ELEMENTTYPE* operator()(DWORD index)
	{
		const TArrayPointer* pThis = static_cast<const TArrayPointer*>(this);
		return const_cast<ELEMENTTYPE*>(pThis->operator()(index));
	}

	ELEMENTTYPE* Detach()
	{
		ELEMENTTYPE* pointer = _pointer;

		_pointer = NULL;
		_count = 0;
		
		return pointer;
	}

private:
	ELEMENTTYPE* _pointer;
	DWORD _count;
};
