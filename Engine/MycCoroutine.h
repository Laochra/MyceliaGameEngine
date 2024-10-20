#pragma once

#include <array>
#include <type_traits>

// USAGE EXAMPLE
//	class ExampleCoroutine : public Coroutine::Function<int> // Base class templated for type safety
//	{
//	public:
//		void operator()(Coroutine::Package& package) // The underlying user-defined function
//		{
//			int& data = GetData(package);
//	
//			if (data < 0)
//			{
//				float x = 1.0f;
//				CoroutineYieldFor(x); // Return for now, and start the function again in x second
//			}
//			else if (data < 50)
//			{
//				data++;
//				CoroutineYield(); // Return for now, and start the function again next frame
//			}
//	
//			CoroutineFinalise(); // Return and declare that the function should not run again
//		}
//	};
//	
//	inline int coroutineData = 50;
//	inline void ExampleCoroutineCaller() noexcept
//	{
//		Coroutine::Start<ExampleCoroutine>(&coroutineData);
//	}

#define CoroutineYield() package.state = Coroutine::Yielding; return
#define CoroutineYieldFor(seconds) package.yieldTimer = (seconds); CoroutineYield()
#define CoroutineFinalise() package.state = Coroutine::Finalised; return

class Coroutine
{
public:
	enum State : char { New, Yielding, Finalised };
	struct Package
	{
		void* data = nullptr;
		float yieldTimer = 0.0f;
		Coroutine::State state = Coroutine::New;
		bool ownsData = false;

		Package() noexcept :
			data(nullptr) {};
		Package(void* dataInit, bool ownsDataInit) noexcept :
			data(dataInit),
			ownsData(ownsDataInit) {};
	};

	class FunctionInterface
	{
	public:
		virtual void operator()(Coroutine::Package& package) = 0;
	};
	template<typename T>
	class Function : public FunctionInterface
	{
	public:
		virtual void operator()(Coroutine::Package& package) = 0;
		T& GetData(Coroutine::Package& package) { return *(T*)package.data; }
	};

	struct Pair
	{
		Coroutine::FunctionInterface* function = nullptr;
		Coroutine::Package package;
	};

	static void Update(float delta) noexcept;

	/// <summary> C must be a Coroutine::Function with a Type matching the Data's Type </summary> <param name="C">The Coroutine Type to be ran</param> <param name="T">The Data Type</param> <param name="data">The Data for the Coroutine</param>
	template<typename C, typename T>
		requires std::is_base_of<Coroutine::Function<T>, C>::value
	static inline const Coroutine::Pair* Start(T* data, bool takePossession) noexcept
	{
		for (int i = 0; i < (int)Coroutine::list.size(); i++)
		{
			if (Coroutine::list[i].function == nullptr)
			{
				Coroutine::list[i].function = new C();
				Coroutine::list[i].package.data = data;
				Coroutine::list[i].package.ownsData = takePossession;
				return &Coroutine::list[i];
			}
		}
		return nullptr;
	}

private:
	static inline std::array<Coroutine::Pair, 1024> list;
};
