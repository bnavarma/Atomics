#include <iostream>
#include <memory>
#include "lockfree_slist.h"
#include "atomic_shared_ptr.h"

int main() {
	std::cout << "Avinash";
	auto x = std::make_unique<slist<int>>();
	auto s1 = std::make_shared<int>(1);
	auto s2 = std::make_shared<int>(2);
	{
		auto p = s1;
		p = s2;
	}

	auto y = s2;
	y = std::move(s1);

	atomic_shared_ptr<int> a(new int(1));
	auto b = a;
	return 0;
}