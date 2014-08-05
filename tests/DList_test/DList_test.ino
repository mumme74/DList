#include <DList.h>
#include <testmacros.h>

class TestClass {
public:
  const char *name;
  TestClass(const char str[]) : name(str) { }
  TestClass(){ name = NULL; } // must have a default empty constructor
};



void runTests(){
	// declare a dynamic array that stores ints
	DList<int> iArr;
	DList<TestClass> tcArr;
	typedef DListNode<int> iNode;
	typedef DListNode<TestClass> tcNode;
	
	testBegin();
	
	//test initialization
	test(iArr.length(), 0);
	test(tcArr.length(), 0);

	// test push()
	iNode i10(10);
	test(iArr.push(i10), 1);
	test(iArr.length(),1);
	test(iArr[0],10);
	
	// test pop()	
	test(iArr.pop(), 10);
	test(iArr.length(), 0);
	test(iArr[0], 0);

	
	// test push_first() 
	iNode i20(20);
	test(iArr.push_first(i20),1);
	test(iArr.length(),1);
	test(iArr[0],20);

	// test pop_front()
	test(iArr.pop_first(),20);
	test(iArr.length(),0);
	test(iArr[0], 0);


	// test insert
	iNode i29 = 29, i30 = 30, i31 = 31, i34 = 34, i35 = 35, i36 = 36;
	test(iArr.insert(0, i30),true);
	test(iArr.length(), 1);
	test(iArr.insert(1, i31),true);
	test(iArr[1], 31);
	test(iArr.length(), 2);
	test(iArr.insert(0, i29),true);
	test(iArr[1], 30);
	test(iArr[2], 31);
	test(iArr.length(), 3);
	test(iArr[0], 29);
	test(iArr[1], 30);
	test(iArr[2], 31);
	test(iArr.insert(1, i34), true);
	test(iArr.length(), 4);
	test(iArr[1], 34);
	test(iArr[2], 30);
	test(iArr[3], 31);
	
	test(iArr.insert(4, i35), true); // insert to pos 1 above last
	test(iArr.length(), 5);
	test(iArr[0], 29);
	test(iArr[1], 34);
	test(iArr[2], 30);
	test(iArr[3], 31);
	test(iArr[4], 35);
	test(iArr.insert(6, i36), false);
	test(iArr.length(), 5);
	

	// test remove arr is now: [29, 34, 30, 31, 35]
	test(iArr.remove(5), false); // remove non existant
	test(iArr.remove(4), true); // remove 35
	test(iArr.length(), 4);
	test(iArr.remove(1), true); // remove 34
	test(iArr.length(), 3);
	test(iArr[2], 31);
	test(iArr[1], 30);
	test(iArr[0], 29);
	test(iArr.remove(2), true); // remove 31
	test(iArr.length(), 2);
	test(iArr[0],29);
	test(iArr[1], 30);
	test(iArr.remove(0), true); // remove 29
	test(iArr.length(), 1);
	test(iArr[0], 30);
	test(iArr.remove(0),true); // remove 30
	test(iArr.length(), 0);
	test(iArr.remove(0), false); // nothing to remove, return false

	// test writeable operator[]
	iNode i40(40);
	test(iArr.push(i40), 1);
	test(iArr[0], 40);
	test(iArr[0] = 45, 45);
	test(iArr[0], 45);
	test(iArr[0]++, 45);
	test(++iArr[0], 47);
	test(iArr[0]--, 47);
	test(--iArr[0], 45);
	test(iArr.pop(), 45);


	// fill values in a loop	
	iNode ia[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	for(size_t i = 0; i < 10; ++i){
		test(iArr.push(ia[i]), i+1);
		test(iArr.length(), i +1);
	}
	
	test(iArr.length(), 10);
	
	// make sure they are in order
	for(int i = 0; i < 10; i++){
		test(iArr[i], i);
	}
	
	// and backwards
	for(int i = 9; i >= 0; --i){
		test(iArr[i], i);
	}

	// test forward iterator ascending order
	int i = 0;
	for(auto vlu = iArr.first(); iArr.canMove(); vlu = iArr.next()) {
		test(vlu, i);
		i++;
	}
	test(/*itercount*/ i, 10);

	// test backward iterator decending order
	i = iArr.length() -1;
	for(auto vlu = iArr.last(); iArr.canMove(); vlu = iArr.previous()){
		test(vlu, i);
		i--;
	}
	test(/*itercount reversed*/ i, -1);

	// test writable iterators
	test(iArr.last() = 20, 20);
	test(iArr[9], 20);
	test(iArr[9] = 9, 9);
	test(iArr.first() = 30, 30);
	test(iArr[0], 30);
	test(iArr[0] = 0, 0);
	test(iArr.length(), 10);
	
	// test indexof
	test(iArr.indexOf(5), 5);
	test(iArr.indexOf(10), -1);
	test(iArr.indexOf(-1), -1);
	test(iArr.indexOf(0), 0);
	test(iArr.indexOf(9), 9);
	
	// test iterators not interfered by removing, inserting objects or lookup items
	test(iArr.first(), 0); // set iterator at iArr[0]
	test(iArr.at(), 0);
	iNode i50(50);
	test(iArr.push_first(i50), 11); // implicitly bump the iterator to arr[1]
	test(iArr.length(), 11);
	test(iArr.next(), 1); 					// set to iArr[2] 
	test(iArr.at(), 2);

	iNode i60 = 60, i65 = 65;
	test(iArr.insert(3, i60), true); // inserting above should not move iterator, still arr[2] 
	test(iArr.next(), 60); 					 // set to arr[3]
	test(iArr.insert(3, i65), true); // implicitly bump to arr[4]
	test(iArr[3], 65);
	test(iArr.length(), 13);				 // should be {50, 0, 1, 65, 60, 2, 3, 4, 5, 6, 7, 8, 9 }
	test(iArr.next(), 2);					 // to arr[5]
	test(iArr[0], 50); 			// array lookup should not interfere with iterposition 
	test(iArr.next(), 3);
	test(iArr[12], 9);
	test(iArr.next(), 4);
	test(iArr[6], 3);				
	test(iArr.next(), 5);						// to arr[8]
	test(iArr.at(), 8);
	test(iArr.previous(), 4);
	test(iArr[6], 3);
	test(iArr.previous(), 3);
	test(iArr[7], 4);
	test(iArr.previous(), 2);			// to arr[5]
	test(iArr.remove(3), true); // remove vlu 65, as iterpos is above the one we remove iterpos must be decreased to point to the right place
	test(iArr.length(), 12);
	test(iArr.next(), 3);					// to arr[5] == 3
	test(iArr.previous(), 2);			
	test(iArr.previous(), 60);
	test(iArr.remove(3), true);  // remove vlu 60, iterPos moved to arr[3]
	test(iArr.length(), 11);
	test(iArr.previous(), 1);		// to arr[2]
	test(iArr.at(), 2);
	test(iArr.next(), 2);
	test(iArr.previous(), 1);
	test(iArr.previous(), 0);
	test(iArr.at(), 1);
	test(iArr.canMove(), true);
	test(iArr.pop_first(), 50);
	test(iArr.at(), 0);
	test(iArr.canMove(), true);
	
	// test writing to a empty value Note!! this is actually an error, code should ever get array values that are out of bounds
	test(iArr[20] = 100, 100);
	test(iArr[20], 0);
	
	
	// tests with String type
	typedef DListNode<String> SNode;
	DList<String> SArr;
	
	SNode s1("s1"), s2("s2"), s3("s3"), s4("s4");
	test(SArr.push(s1), 1);
	test(SArr.push(s2), 2);
	test(SArr.push(s3), 3);
	test(SArr.push(s4), 4);
	testS(SArr[0], "s1");
	testS(SArr[1], "s2");
	testS(SArr[2], "s3");
	testS(SArr[3], "s4");
	test(SArr.length(), 4);
	{
	char tmp[] = "s1 this is a much longer string that hopefully expands the storage for String";
	testS(SArr[0] = tmp, tmp);
	testS(SArr[0], tmp);
	}
	test(SArr.remove(2), true);
	testS(SArr[2], "s4");
	
	SArr.insert(2, s3);
	SArr[0] = "s1";
	testS(SArr[0], "s1");
	testS(SArr[1], "s2");
	testS(SArr[2], "s3");
	testS(SArr[3], "s4");
	
	int cnt = 0; i = 1;
	for(auto s = SArr.first(); SArr.canMove();s = SArr.next(), ++i){
		testS(s, String("s") + i);
		++cnt; 
	}
	
	test(cnt, 4);
	
	DList<char> cArr;
	typedef DListNode<char> cNode;
	cNode c0 = '0', c1 = '1';
	// empty array
	i = 0;
	for(auto c = cArr.first(); cArr.canMove(); c = cArr.next()){
		test(c - 48, i);
		i++;
	} 
	test(i, 0);
	
	// one element
	cArr.push(c0);
	i = 0;
	for(auto c = cArr.first(); cArr.canMove(); c = cArr.next()){
		test(c - 48, i);
		i++;
	} 
	test(i, 1);
	
	// two elements
	cArr.push(c1);
	i = 0;
	for(auto c = cArr.first(); cArr.canMove(); c = cArr.next()){
		test(c - 48, i);
		i++;
	}
	test(i, 2);
	
	testEnd();
}

void setup(){
	
	Serial.begin(115200);	
	
}

void loop(){
	testLoop();
}
