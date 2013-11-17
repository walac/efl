
#include "Eina.hh"

#include <cassert>
#include <algorithm>
#include <functional>

#include <check.h>

START_TEST(eina_cxx_inarray_push_back)
{
  efl::eina::eina_init eina_init;

  efl::eina::inarray<int> array;

  array.push_back(5);
  array.push_back(10);
  array.push_back(15);

  int result[] = {5, 10, 15};

  assert(array.size() == 3);
  assert(std::equal(array.begin(), array.end(), result));
}
END_TEST

START_TEST(eina_cxx_inarray_pop_back)
{
  efl::eina::eina_init eina_init;

  efl::eina::inarray<int> array;

  array.push_back(5);
  array.push_back(10);
  array.push_back(15);
  array.pop_back();

  int result[] = {5, 10};

  assert(array.size() == 2);
  assert(std::equal(array.begin(), array.end(), result));
}
END_TEST

// START_TEST(eina_cxx_inarray_push_front)
// {
//   efl::eina::eina_init eina_init;

//   efl::eina::inarray<int> array;

//   array.push_front(5);
//   array.push_front(10);
//   array.push_front(15);

//   int result[] = {15, 10, 5};

//   assert(array.size() == 3);
//   assert(std::equal(array.begin(), array.end(), result));
// }
// END_TEST

// START_TEST(eina_cxx_inarray_pop_front)
// {
//   efl::eina::eina_init eina_init;

//   efl::eina::inarray<int> array;

//   array.push_front(5);
//   array.push_front(10);
//   array.push_front(15);
//   array.pop_front();

//   int result[] = {10, 5};

//   assert(array.size() == 2);
//   assert(std::equal(array.begin(), array.end(), result));
// }
// END_TEST

START_TEST(eina_cxx_inarray_insert)
{
  efl::eina::eina_init eina_init;

  efl::eina::inarray<int> array;

  efl::eina::inarray<int>::iterator it;

  it = array.insert(array.end(), 5); // first element
  assert(it != array.end());
  ++it;
  assert(it == array.end());

  it = array.insert(array.end(), 10);  // equivalent to push_back
  assert(it != array.end());
  ++it;
  assert(it == array.end());

  it = array.insert(array.begin(), 15); // equivalent to push_front
  assert(it == array.begin());

  it = array.end();
  --it;
  array.insert(it, 20); // insert before the last element

  int result[] = {15, 5, 20, 10};

  assert(array.size() == 4);
  assert(std::equal(array.begin(), array.end(), result));

  efl::eina::inarray<int> array2;
  it = array2.insert(array2.end(), array.begin(), array.end());
  assert(it == array2.begin());
  assert(array == array2);

  efl::eina::inarray<int> array3;
  array3.push_back(1);
  it = array3.insert(array3.end(), array.begin(), array.end());
  assert(array3.size() == 5);
  assert(array3.front() == 1);
  it = array3.begin();
  ++it;
  assert(std::equal(it, array3.end(), array.begin()));

  efl::eina::inarray<int> array4;
  array4.push_back(1);
  it = array4.insert(array4.begin(), array.begin(), array.end());
  assert(array4.size() == 5);
  assert(array4.back() == 1);
  assert(std::equal(array.begin(), array.end(), array4.begin()));
}
END_TEST

START_TEST(eina_cxx_inarray_constructors)
{
  efl::eina::eina_init eina_init;

  efl::eina::inarray<int> array1;
  assert(array1.empty());

  efl::eina::inarray<int> array2(10, 5);
  assert(array2.size() == 10);
  assert(std::find_if(array2.begin(), array2.end()
                      , std::not1(std::bind1st(std::equal_to<int>(), 5))) == array2.end());

  efl::eina::inarray<int> array3(array2);
  assert(array2 == array3);

  efl::eina::inarray<int> array4(array2.begin(), array2.end());
  assert(array2 == array4);
}
END_TEST

START_TEST(eina_cxx_inarray_erase)
{
  efl::eina::eina_init eina_init;

  efl::eina::inarray<int> array1;
  array1.push_back(5);
  array1.push_back(10);
  array1.push_back(15);
  array1.push_back(20);
  array1.push_back(25);
  array1.push_back(30);
  
  efl::eina::inarray<int>::iterator it = array1.begin(), it2;

  it = array1.erase(it);
  assert(it == array1.begin());
  assert(array1.size() == 5);
  assert(array1.front() == 10);

  it = array1.begin() + 1;
  assert(*it == 15);
  it = array1.erase(it);
  assert(*it == 20);
  assert(array1.size() == 4);
  
  it = array1.end() - 1;
  it = array1.erase(it);
  assert(it == array1.end());
  assert(array1.size() == 3);
  assert(array1.back() == 25);

  it = array1.begin() + 1;
  it2 = array1.end() - 1;
  it = array1.erase(it, it2);
  it2 = array1.end() -1;
  assert(it == it2);
  assert(array1.size() == 2);
  assert(array1.front() == 10);
  assert(array1.back() == 25);
}
END_TEST

void
eina_test_inarray(TCase *tc)
{
  tcase_add_test(tc, eina_cxx_inarray_push_back);
  tcase_add_test(tc, eina_cxx_inarray_pop_back);
  tcase_add_test(tc, eina_cxx_inarray_insert);
  tcase_add_test(tc, eina_cxx_inarray_erase);
  tcase_add_test(tc, eina_cxx_inarray_constructors);
}
