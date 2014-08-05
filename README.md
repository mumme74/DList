DList
=====

A linked list class for Arduino with access functions and iteration helpers. This is because Arduino (Teensy) doesnt like std::list as it doesnt have a stndard lib by default (takes to much space)


You use it much like you use a array.
As we dont use any HEAP (objects created on the fly with new) you must provide storage for the listnode on the stack
Just as you normally do in your programs


example:
````
class MyClass : public DListNode<MyClass*> // inherit DListNode so we dont have to create an explicit listnode
{
public: 
  MyClass() : DListNode<MyClass*>(this) // set the DListNode pointer to point to this class
  {}
  void doSomethingFancy();
}

DList<MyClass *> myclasses; // set myclasses to take MyClasses pointers

myclasses.length(); // == 0 as it empty

MyClass fancy1, fancy2, fancy3; // create 2 objects on the stack
myclasses.push(fancy1);
myclasses.push(fancy2); // store these
myclasses.push(fancy3)
myclasses.length(); // == 3 as we now have 3 objects stored

myclasses[0]; // == fancy1
myclasses[2]; // == fancy3

MyClass more1, more2, more3; // create some more object on the stack
myclasses.push_first(more1); // push first in list

myclasses[0]; // == more1
myclasses[1]; // == fancy1 as we pushed more1 on fancy1´s previous place

myclasses.insert(1, more2); // insert in the middle (on position 1), takes fancy1´s position
myclasses[2];   // == fancy1 as it now have been pushed back twice

myclasses[2] = more3;  // replaces fancy1 with more3

myclasses.remove(2);  // removes more3 from the list

myclasses.pop();  // takes out last element from the list and returns it (fancy3)

myclasses.pop_first(); // takes out first element from the list and returns it (more1)

//iterate
for (int i = 0; i < myclasses.length(); i++){
  myclasses[i];   // do something with each element in the list
}

// or with iterator helpers
for (auto cls = myclasses.first(); myclasses.canMove(); cls = myclasses.next()){
  cls->doSomethingFancy(); // we dont need to derefernce the class each time, we can just use it
}

// same but backwards (starting from the back instead)
for (auto cls = myclasses.last(); myclasses.canMove(); cls = myclasses.previous()){
  cls->doSomethingFancy();  // in this loop the last element gets called first
}

````

Of course it also works on ints and other primitive types, however you must also create a DListNode<int> for each value as we DList trust you to do the storage for its items.
````
DListNode<Int> i1 = 1, i2 = 2; // note that you must create a DListNode that can store your int
DList<int> arr;
arr.push(i1);
arr.push(i2); 
````



<b>NOTE!!</b>
You must watch out for scope lifetime of your listnodes
The list dosnt copy anything, its you that must store each DListNode
If you store a DListNode that goes out of scope your program will crash

````
DList<int> arr;

// some code inbetween....

if (something) {
  DListNode<int> i2 = 2; // WRONG!! creating a listnode in a deeper scope than arr
  arr.push(i2);
} // i2 gets out of scope here as the curly brace ends here, 
  // result memory place get collected and overwritten by other things

// some rows later ....

int x = arr[0]; // the internal pointers needed by DList has ben overwritten and points elsewere
                // you are probably saying WTH happend now ??

````


<b>API</b>
_DList&lt;typename&gt;_ 
Is the root class, its the one that you are using to push, access, remove etc on your array
_typename_ names the type that the array should hold such as _&lt;int&gt;_ or _&lt;widget *&gt;_

_DListNode&lt;typename&gt;_
Is the storage class you should only use this to store your value. _typename_ must be the same as you constructed your _DList_ with.





