#ifndef DLIST_H
#define DLIST_H

#include <limits.h>
#include <stddef.h>
#include <Arduino.h>

/**
* a dynamic array, not particulary fast or optimized its actually a linked list but it does 
* support adding, removing and accessing element with push and pop,
* push_begin() and pop_begin(), accesses with at, insert, remove, and []
* iteration helpers begin, next and atEnd (no true iterators)
* it has no but no sparse array support
* 
*  But it does not have any memory manegment, thats up to the user
*/

template<typename T>
class DList;

template<typename T>
class DListNode {
	friend class DList<T>;
	DListNode *_next;
	DListNode *_previous;
protected:
	T _value;
public:
	DListNode(T vlu) : _next(0), _previous(0), _value(vlu) {}
	DListNode() : _next(0), _previous(0), _value() {}
	T &operator= (T vlu) { _value = vlu; return _value; }
};


template<typename T>
class DList{ // on stack version doesnt use "new " ie user must create a Node object themselves
public:
	// a container for the internal linked list
	typedef DListNode<T> Node;
	
protected:
	T _emptyStore; //create memory storage so we can return a empty value when needed (idx out of bound)
								 // this is needed as we use return by reference instead of pointers
	Node *_first;
	Node *_last;
	Node *_iterPos;
	size_t _iterPosIdx;
	Node *_seekPos;
	size_t _seekPosIdx;
	size_t _len; // the last inserted index +1 
	
	T &_empty(){
		// returns a empty value, make sure it is empty each time because we our stored value
		T userType = T();
		_emptyStore = userType; // copy data from the newly created 
		return _emptyStore;
	}
	
	// get the value at idx, and set the internal _iterPos
	T &_seek(size_t idx){
		if (idx >= _len) // bail out if array is empty or we are out of range
			return _empty();
			
		// try to find it by searching the list from next pos 
		// from the previously accessed Node
		// typically from a iteration loop
		if (idx == _seekPosIdx + 1 && _seekPos != 0){
			// typically in a:
			// 		for(int i = 0; i < arr.length(); i++)
			//			dosomething(arr.at(i));
			//Serial.println("_seek is pos +1");
			_seekPos = _seekPos->_next;
			++_seekPosIdx;
		} else if (idx > 0 && idx == _seekPosIdx - 1) {
			// same as above but a decreasing iteration instead (ie i-- instead)
			//Serial.println("_seek is pos -1");
			_seekPos = _seekPos->_previous;
			--_seekPosIdx;
		} else if (idx < (_len / 2)) {
			//Serial.println("_seek from beginning");
			// nearer to beginning than end
			_seekPos = _first; _seekPosIdx = 0;
			while(_seekPosIdx < idx) {
				_seekPos = _seekPos->_next;
				++_seekPosIdx;
			}
		} else {
			//Serial.println("_seek from end");
			// nearer end than beginning
			_seekPos = _last; _seekPosIdx = _len -1;
			while (_seekPosIdx > idx){
				_seekPos = _seekPos->_previous;
				--_seekPosIdx;
			}
		}
		return _seekPos->_value;
	}

public:
	DList() : _emptyStore(), _first(0), _last(0), _iterPos(0), _iterPosIdx(0), _seekPos(0), _seekPosIdx(0),  _len(0) {}
	~DList(){}
	typedef DListNode<T> N;
	
	// return the length of the array
	size_t length() const { return _len;}
	
	// insert one at end return new length
	size_t push(Node &n){
		if (_first == 0){
			_first = &n;
		} else {
			n._previous = _last;
			_last->_next = &n;
		}
		_last = &n;
		return ++_len;
	}
	// remove last and returned the removed (If empty it returns NULL)
	T &pop(){
		if (_last != 0) {
			if (_iterPos == _last){
				_iterPos = _last->_previous; 
				--_iterPosIdx; 
			}
			if (_seekPos == _last) {
				_seekPos = _last->_previous;
				--_seekPosIdx;
			}
			T *tmp = &_last->_value;
			_last = _last->_previous;
			if (_last == 0) {
				_last = _first = 0;// empty again
			} else {
				_last->_next = 0; // make this the last node 
			}
			_len--;
			return *tmp;
		}
		return _empty();
	}
	// insert one at beginning return new length
	size_t push_first(Node &n){
		if (_first != 0){
			_first->_previous = &n;
			++_seekPosIdx;
			++_iterPosIdx;
		} else {
			_last = &n;
		}
		n._next = _first;
		_first = &n;
		return ++_len;
	}
	// remove the first one and return it
	T &pop_first(){
		if (_first != 0){
			if (_iterPos == _first)
				_iterPos = _first->_next;
			else if (_iterPosIdx > 0)
				--_iterPosIdx;
			if (_seekPos == _first)
				_seekPos = _first->_next;
			else 
				--_seekPosIdx;
				
			T *tmp = &_first->_value;
			_first = _first->_next;
			if (_first != 0) {
				_first->_previous = 0;
			} else {
				_last = 0; // empty again
			}
			_len--;
			return *tmp;
		}
		return _empty();
	}

	//removes idx from array, returns true if successfull
	bool remove(size_t idx){
		if (idx >= _len)
			return false;
		
		// find idx in array
		_seek(idx);
		
		if (_seekPos == _first){
			//Serial.println("in remove first");
			pop_first();
		} else if (_seekPos == _last){
			 // we should never get here when array is about to be empty again
			// Serial.println("in remove last");
			pop();
		} else {
			//Serial.println("in remove middle");
			// in middle of array, take _iterPos out of the chain
			Node *next = _seekPos->_next;
			Node *previous = _seekPos->_previous;
			next->_previous = previous;
			previous->_next = next;
			
			if (_iterPos == _seekPos) { _iterPos = _iterPos->_next; }
			_seekPos = _seekPos->_next;
			
			// decrease length counter
			_len--;
					
			// handle the iterator pos
			if (_iterPosIdx > idx){ --_iterPosIdx; }
		}
		return true;
	}
	// inserts new value at index shifting exising value up one place,
	//  returns false if it could not 
	// ie when we try to set a new value at out of bound idx
	bool insert(size_t idx, Node &n){
		if (_len < idx) {
			return false; // we dont support sparse arrays
		} else if (_len == idx) {
			//Serial.println("in insert at last +1");
			push(n);
		} else if (idx == 0) {
			//Serial.println("in insert at pos[0]");
			push_first(n);
		} else {
			//Serial.println("in insert in middle");
			// find the node and insert it
			_seek(idx);
			n._previous = _seekPos->_previous;
			n._next = _seekPos;
			_seekPos->_previous->_next = &n;
			_seekPos->_previous = &n;
			++_seekPosIdx;
			
			// handle the iterator pos
			if (_iterPosIdx >= idx){ ++_iterPosIdx; }
			++_len;
		}
		
		return true;
	}
	// this is how you get the values from the array
	// makes it possible to "vlu = arr[i];"
	// and "arr[i] = vlu; because we return a reference"
	// it wont insert new Nodes however, use push() for that
	T &operator[](size_t idx) { return _seek(idx); }
	
	// returns the first index of this node or -1 if not found
	int32_t indexOf(const T &vlu) {
		if (_first != 0) {
			size_t i = 0;
			do {
				_seek(i);
				if (_seekPos && _seekPos->_value == vlu){
					return static_cast<int32_t>(i);
				}
				++i;
			} while(i <= _len);
			_seek(i);
			
		}
		return -1;
	}
	
	
	
	// iteration helpers 
	// usage: for(auto vlu = arr.first(); !arr.atLast(); vlu = next())
	//          dosomething(vlu);
	T &first(){ _iterPos = _first; _iterPosIdx = 0; return _seek(_iterPosIdx); }
	T &next(){
		if (_iterPos != 0){
			 _iterPos = _iterPos->_next;
	    ++_iterPosIdx;
			if (_iterPos != 0){
				return _iterPos->_value;
			}
		}
		return _empty(); 
	}
	T &begin() { return first(); }
	T &end() { return _empty(); }
	// this ends a iter loop
	const bool canMove() const { return _iterPos != 0; }
	
	// get the internal iteration pos pointer from
	const int at() const { return _iterPos != 0 ? _iterPosIdx : -1; }
	
	// iteration helper for moving downwards
	// usage: for(auto item = items.last(); !items.atFirst(); vlu = items.previous()) 
	//          dosomething(item); 
	T &last(){ _iterPos = _last; _iterPosIdx = _len - 1; return _seek(_iterPosIdx);	}
	T &previous() {
		if (_iterPos != 0) {
		  _iterPos = _iterPos->_previous;
		  --_iterPosIdx;
			if (_iterPos != 0){		
		  	return _iterPos->_value;
			}
		}	
		return _empty();
	}
	T &rbegin(){ return last(); }
	T &rend(){ return _empty(); }
};

/// helper class, handles Nodes new delete
template<typename T>
class DListDynamic : public DList<T>
{
public:
  typedef DListNode<T> Node;
  DListDynamic() : DList<T>::DList() {}
  ~DListDynamic() { DList<T>::~DList(); }
  size_t push(T vlu) {
    Node *n = new Node(vlu);
    return DList<T>::push(*n);
  }
  T &pop() {
    Node *n = DList<T>::_last;
    T vlu = DList<T>::pop();
    if (n) delete n;
    return vlu;
  }
  size_t push_first(T vlu) {
    Node *n = new Node(vlu);
    return DList<T>::push_first(n);
  }
  T &pop_first(){
    Node *n = DList<T>::_first;
    T vlu = DList<T>::pop_first();
    if (n) delete n;
    return vlu;
  }
  bool remove(size_t idx){
    if (idx >= DList<T>::_len)
      return false;

    DList<T>::_seek(idx);
    Node *n = DList<T>::_seekPos;

    bool res = DList<T>::remove(idx);
    if (n) delete n;
    return res;
  }
  bool insert(size_t idx, T vlu){
    Node *n = new Node(vlu);
    bool res = DList<T>::insert(idx, *n);
    if (!res)
      delete n;
    return res;
  }

};


#endif // DLIST_H
