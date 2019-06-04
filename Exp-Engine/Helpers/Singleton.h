#pragma once
#include <memory>
namespace Exp
{
	template<typename T>
	class Singleton
	{
	protected:
		//struct token {};
		Singleton() {}
	public:
		
		static T & Get();

		Singleton(const Singleton&) = delete;
		Singleton& operator= (const Singleton) = delete;
	};

	template<typename T>
	T& Singleton<T>::Get()
	{
		static T instance;
		return instance;
	}
}