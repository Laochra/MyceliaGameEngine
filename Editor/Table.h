// Example of how to use:
// Table<3> table("MyTable", ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders,
//		{ "Name", "Body", "ID" },
//		{
//			{"1", "2", "3"},
//			{"4", "5", "6"},
//			{"7", "8", "9"}
//		});
//		table.Draw();

#include "ImGuiIncludes.h"

namespace GUI
{
	typedef unsigned int uint;

	template<uint columnCount>
	struct Table
	{
		using Row = std::array<const char*, columnCount>;
		using RowIni = std::initializer_list<const char*>;
		using MultiRowIni = std::initializer_list<RowIni>;

		const char* name;
		ImGuiTableFlags flags;

		const char* headerRow[columnCount];

		vector<std::array<const char*, columnCount>> rows;

		Table(const char* nameInit, ImGuiTableFlags flagsInit, const char* headerRowInit[columnCount], vector<const char* [columnCount]> rowsInit = vector<const char* [columnCount]>()) noexcept;
		Table(const char* nameInit, ImGuiTableFlags flagsInit, RowIni headerRowInit, MultiRowIni rowsInit = MultiRowIni()) noexcept;

		void AddRow(vector<const char* [columnCount]> rowInit) noexcept;
		void AddRow(RowIni rowInit) noexcept;

		void Draw() const noexcept;
	};

	template<uint columnCount>
	inline Table<columnCount>::Table(const char* nameInit, ImGuiTableFlags flagsInit, const char* headerRowInit[columnCount], vector<const char* [columnCount]> rowsInit) noexcept
	{
		name = nameInit;
		flags = flagsInit;

		for (uint column = 0; column < columnCount; column++)
		{
			headerRow[column] = headerRowInit[column];
		}

		for (uint row = 0; row < rowsInit.size(); row++)
		{
			Table::Row newRow;
			for (uint column = 0; column < columnCount; column++)
			{
				newRow[column] = rowsInit[row][column];
			}
			rows.push_back(newRow);
		}
	}
	template<uint columnCount>
	inline Table<columnCount>::Table(const char* nameInit, ImGuiTableFlags flagsInit, Table::RowIni headerRowInit, Table::MultiRowIni rowsInit) noexcept
	{
		assert(headerRowInit.size() == columnCount);

		name = nameInit;
		flags = flagsInit;

		for (uint column = 0; column < columnCount; column++)
		{
			headerRow[column] = headerRowInit.begin()[column];
		}

		for (uint row = 0; row < rowsInit.size(); row++)
		{
			Table::Row newRow;
			for (uint column = 0; column < columnCount; column++)
			{
				newRow[column] = rowsInit.begin()[row].begin()[column];
			}
			rows.push_back(newRow);
		}
	}

	template<uint columnCount>
	inline void Table<columnCount>::AddRow(vector<const char* [columnCount]> rowInit) noexcept
	{
		Table::Row newRow;
		for (uint column = 0; column < columnCount; column++)
		{
			newRow[column] = rowInit[column];
		}
		rows.push_back(newRow);
	}
	template<uint columnCount>
	inline void Table<columnCount>::AddRow(RowIni rowInit) noexcept
	{
		Table::Row newRow;
		for (uint column = 0; column < columnCount; column++)
		{
			newRow[column] = rowInit.begin()[column];
		}
		rows.push_back(newRow);
	}

	template<uint columnCount>
	inline void Table<columnCount>::Draw() const noexcept
	{
		ImGui::BeginTable(name, columnCount, flags);

		for (uint column = 0; column < columnCount; column++)
		{
			ImGui::TableSetupColumn(headerRow[column]);
		}

		ImGui::TableHeadersRow();

		for (uint row = 0; row < rows.size(); row++)
		{
			ImGui::TableNextRow();

			for (uint column = 0; column < columnCount; column++)
			{
				if (ImGui::TableNextColumn()) ImGui::Text(rows[row][column]);
			}
		}

		ImGui::EndTable();
	}
}