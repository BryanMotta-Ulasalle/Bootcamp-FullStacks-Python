#include "symtable.h"

// constructor para la primera tabla
SymTable::SymTable() : prev(nullptr)
{
}

// constructor para nuevas tablas
SymTable::SymTable(SymTable * t) : prev(t)
{		
}

// inserta un símbolo en la tabla
bool SymTable::Insert(string s, Symbol symb) 
{ 
	const auto& [pos, success] = table.insert({s,symb});
	return success;
}

// busca un símbolo en la tabla actual o en las de los ámbitos envolventes
Symbol * SymTable::Find(string s) 
{
	for (SymTable * st = this; st != nullptr; st = st->prev) 
	{
        auto found = st->table.find(s);
        if (found != st->table.end()) 
			return &found->second;
    }

    return nullptr;
} 

