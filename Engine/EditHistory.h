#pragma once

#include <vector>
#include "JsonIncludes.h"

#include "GameObject.h"
#include "GameObjectManager.h"

#include <type_traits>
#include "Debug.h"

typedef unsigned long long ulong;

class EditHistory
{
public:
	template<typename ValType>
	struct Value
	{
		const ValType value;
		Value(ValType valueInit) : value(valueInit) { }
	};
	struct Edit
	{
		virtual bool Finalise() noexcept = 0;

		virtual void Undo() noexcept = 0;
		virtual void Redo() noexcept = 0;
	};
	template<typename ValType>
	struct EditValue : public Edit
	{
		void* valuePtr = nullptr;
		Value<ValType>* before = nullptr;
		Value<ValType>* after = nullptr;

		EditValue(ValType& val) noexcept;
		~EditValue() noexcept;
		bool Finalise() noexcept override;

		void Undo() noexcept override;
		void Redo() noexcept override;
	};
	struct EditGameObject : public Edit
	{
		struct ParentingData
		{
			GameObject3D* oldParent = nullptr; ulong oldIndex = 0;
			GameObject3D* newParent = nullptr; ulong newIndex = 0;
		};
		GameObject*& gameObject;
		json* before = nullptr;
		json* after = nullptr;
		ParentingData parentingData;

		EditGameObject() noexcept = default;
		EditGameObject(GameObject*& gameObjectInit) noexcept;
		~EditGameObject() noexcept;
		bool Finalise() noexcept override;
		
		void Undo() noexcept override;
		void Redo() noexcept override;
	};

	template<typename ValType>
	static void Begin(ValType&& valueToMonitor) noexcept;
	static void End() noexcept;
	
	static bool CanUndo() noexcept;
	static bool CanRedo() noexcept;
	static void Undo() noexcept;
	static void Redo() noexcept;

private:
	static Edit* currentWatch;
	static vector<Edit*> historyStack;
	static ulong currentStackIndex;

};

#pragma region struct EditHistory::EditValue
	template<typename ValType>
	inline EditHistory::EditValue<ValType>::EditValue(ValType& val) noexcept
	{
		valuePtr = &val;
		before = new Value<ValType>(val);
	}
	template<typename ValType>
	inline EditHistory::EditValue<ValType>::~EditValue() noexcept
	{
		delete before;
		delete after;
	}
	template<typename ValType>
	inline bool EditHistory::EditValue<ValType>::Finalise() noexcept
	{
		after = new Value<ValType>(*(ValType*)valuePtr);
		return true;
	}
	
	template<typename ValType>
	inline void EditHistory::EditValue<ValType>::Undo() noexcept
	{
		*(ValType*)valuePtr = ((Value<ValType>*)before)->value;
	}
	template<typename ValType>
	inline void EditHistory::EditValue<ValType>::Redo() noexcept
	{
		*(ValType*)valuePtr = ((Value<ValType>*)after)->value;
	}
#pragma endregion

template<typename ValType>
inline void EditHistory::Begin(ValType&& valueToMonitor) noexcept
{
	if (currentWatch != nullptr)
	{
		Debug::LogWarning("EditHistory::Begin/End() Mismatch! End the current edit before beginning a new one.", locationinfo);
		return;
	}

	if constexpr (requires { valueToMonitor->GetGUID(); })
	{
		currentWatch = new EditGameObject((GameObject*&)valueToMonitor);
	}
	else
	{
		currentWatch = new EditValue(valueToMonitor);
	}
}

