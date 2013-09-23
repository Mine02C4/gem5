#include "mem/ruby/network/garnet/fixed-pipeline/RoutingTableEntry.hh"

RoutingTableEntry::RoutingTableEntry(const Params *p)
	: SimObject(p)
{
	m_cur_router = p->cur_router;
	m_dest_router = p->dest_router;
	m_invc = p->invc;
	m_next_router = p->next_router;
	m_outvc = p->outvc;
}

RoutingTableEntry *
RoutingTableEntryParams::create()
{
	return new RoutingTableEntry(this);
}
