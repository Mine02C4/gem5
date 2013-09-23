#ifndef __MEM_RUBY_NETWORK_GARNET_FIXED_PIPELINE_ROUTINGTABLEENTRY_HH__
#define __MEM_RUBY_NETWORK_GARNET_FIXED_PIPELINE_ROUTINGTABLEENTRY_HH__

#include "params/RoutingTableEntry.hh"
#include "sim/sim_object.hh"

class RoutingTableEntry : public SimObject
{	
  public:
	typedef RoutingTableEntryParams Params;
	RoutingTableEntry(const Params *p);
	int get_cur_router() const { return m_cur_router; }
	int get_dest_router() const { return m_dest_router; }
	int get_invc() const { return m_invc; }
	int get_next_router() const { return m_next_router; }
	int get_outvc() const { return m_outvc; }	

  private:
	int m_cur_router, m_dest_router, m_invc, m_next_router, m_outvc;

};

#endif
