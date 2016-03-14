#pragma once
#include <atomic>
#include <memory>

template<typename T>
class slist {
	struct Node {
		T t;
		std::shared_ptr<Node> next;
	};

	std::shared_ptr<Node> head;

public:
	class reference {
		std::shared_ptr<Node> p;
	public:
		reference(std::shared_ptr<Node> p_):p(p_) {}
		T& operator*() { return p->t; }
		T* operator->() { return &p->t; }
	};

	auto find(T t) const {
		auto p = std::atomic_load(&head);
		while (p && p->t != t) {
			p = p->next;
		}
		return reference(std::move(p));
	}

	void push_front(T t) {
		auto p = std::make_shared<Node>();
		p->t = t;
		p->next = head;
		while (!std::atomic_compare_exchange_weak(&head, p->next, p)) {}
	}

	void pop_front() {
		auto p = std::atomic_load(&head);
		while (p &&  !std::atomic_compare_exchange_weak(&head,p, p->next)) {}
	}
};
