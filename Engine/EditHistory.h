#pragma once

#include <vector>
#include "JsonIncludes.h"

#include "GameObject.h"
#include "GameObjectManager.h"

#include <type_traits>
#include <concepts>

template <typename T>
concept HasEquals = requires(T t1, T t2) { t1 = t2; };

template<typename T> concept NonGameObjectPtrType =
	std::is_pointer_v<std::remove_reference_t<T>> &&
	!std::is_base_of_v<GameObject, std::remove_pointer_t<std::remove_reference_t<T>>>;
template<typename T> concept NonPtrTypeWEquals =
	!std::is_pointer_v<std::remove_reference_t<T>> &&
	HasEquals<T>;
template<typename T> concept NonPtrTypeNoEquals =
	!std::is_pointer_v<std::remove_reference_t<T>> &&
	!HasEquals<T>;

#include "GeneralMacros.h"

#include "Debug.h"

typedef unsigned long long ulong;

//	USAGE GUIDE
// 1. Use EditHistory::Begin() to monitor a value for changes.
// 2. Use EditHistory::End() to finalise the monitoring. If the value changed it will be added to the edit stack.
// 3. Use EditHistory::Undo() to move back one step in the edit stack. Always check EditHistory::CanUndo() first.
// 4. Use EditHistory::Redo() to move forward one step edit stack. Always check EditHistory::CanRedo() first.
// 5. Use EditHistory::Clear() to remove all edits from the edit stack.
// IMPORTANT:
// This should only be used on values that will exist for the lifetime of this object, or until the next EditHistory::Clear(). There is no handling of dangling pointers, that is on the user to be mindful of.
class EditHistory
{
public:
	struct Edit
	{
		virtual bool Finalise() noexcept = 0;

		virtual void Undo() noexcept = 0;
		virtual void Redo() noexcept = 0;
	};
	template<NonPtrTypeWEquals ValType>
	struct EditValue : public Edit
	{
		ValType& valueRef; ValType before, after;

		EditValue(ValType& val) noexcept;
		bool Finalise() noexcept override;

		void Undo() noexcept override;
		void Redo() noexcept override;
	};
	struct EditGameObject : public Edit
	{
		// UNION
		// 1. gameObjectEditRef:
		//      Only active before edit is finalised.
		//      DO NOT USE IN UNDO/REDO!
		// 2. guid:
		//      Only active after edit is finalised.
		//      DO NOT USE ON CURRENT EDITS!
		union { GameObject* gameObjectEditRef; ulong guid = 0; };

		struct ParentingData
		{
			ulong oldParentGUID = 0; ulong oldIndex = 0;
			ulong newParentGUID = 0; ulong newIndex = 0;
		};
		
		json before, after;
		ParentingData parentingData;

		EditGameObject() noexcept = default;
		EditGameObject(GameObject* gameObjectInit) noexcept;
		bool Finalise() noexcept override;
		
		void Undo() noexcept override;
		void Redo() noexcept override;
	};

	template<NonPtrTypeWEquals ValType>
	void Begin(ValType& valueToMonitor) noexcept;
	void Begin(GameObject* valueToMonitor) noexcept;
	void End() noexcept;
	
	void Clear() noexcept;

	bool CanUndo() noexcept;
	bool CanRedo() noexcept;
	void Undo() noexcept;
	void Redo() noexcept;

	// Deleted Functions with Helpful Errors

	template<NonPtrTypeWEquals ValType>
	deletewithmsg("EditHistory::Begin() can't be used on temporary values.")
	// DELETED FUNCTION: EditHistory::Begin() can't be used on temporary values.
	static void Begin(ValType&& valueToMonitor) noexcept;
	template<NonGameObjectPtrType ValType>
	deletewithmsg("EditHistory::Begin() can't be used on pointers as it has no way to handle dangling pointers. A way around this is to instead provide a data structure that manages the pointer.")
	// DELETED FUNCTION: EditHistory::Begin() can't be used on pointers as it has no way to handle dangling pointers. A way around this is to instead provide a data structure that manages the pointer.
	static void Begin(ValType valueToMonitor) noexcept;
	template<NonPtrTypeNoEquals ValType>
	deletewithmsg("EditHistory::Begin() can't be used on types that don't have the = operator defined.")
	// DELETED FUNCTION: EditHistory::Begin() can't be used on types that don't have the = operator defined.
	static void Begin(ValType valueToMonitor) noexcept;

private:
	Edit* currentEdit = nullptr;
	vector<Edit*> editStack = vector<Edit*>();;
	ulong currentStackIndex = 0Ui64;
};

#pragma region struct EditHistory::EditValue
	template<NonPtrTypeWEquals ValType>
	inline EditHistory::EditValue<ValType>::EditValue(ValType& val) noexcept :
		valueRef(val)
	{
		before = valueRef;
	}
	template<NonPtrTypeWEquals ValType>
	inline bool EditHistory::EditValue<ValType>::Finalise() noexcept
	{
		after = valueRef;
		return true;
	}
	
	template<NonPtrTypeWEquals ValType>
	inline void EditHistory::EditValue<ValType>::Undo() noexcept
	{
		valueRef = before;
	}
	template<NonPtrTypeWEquals ValType>
	inline void EditHistory::EditValue<ValType>::Redo() noexcept
	{
		valueRef = after;
	}
#pragma endregion

template<NonPtrTypeWEquals ValType>
inline void EditHistory::Begin(ValType& valueToMonitor) noexcept
{
	if (currentEdit != nullptr)
	{
		Debug::LogWarning("EditHistory::Begin/End() Mismatch! End the current edit before beginning a new one.", locationinfo);
		return;
	}

	currentEdit = new EditValue(valueToMonitor);
}
inline void EditHistory::Begin(GameObject* valueToMonitor) noexcept
{
	if (currentEdit != nullptr)
	{
		Debug::LogWarning("EditHistory::Begin/End() Mismatch! End the current edit before beginning a new one.", locationinfo);
		return;
	}

	currentEdit = new EditGameObject(valueToMonitor);
}
