/*
 * Copyright (c) 2008 Princeton University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Niket Agarwal
 */

#include "base/cast.hh"
#include "mem/ruby/network/garnet/fixed-pipeline/InputUnit_d.hh"
#include "mem/ruby/network/garnet/fixed-pipeline/Router_d.hh"
#include "mem/ruby/network/garnet/fixed-pipeline/RoutingUnit_d.hh"
#include "mem/ruby/slicc_interface/NetworkMessage.hh"

/*
   Customize routing
   Written by kagami
*/
#include "mem/ruby/network/garnet/fixed-pipeline/RoutingTableEntry.hh"
#include "debug/RubyNetwork.hh"

RoutingUnit_d::RoutingUnit_d(Router_d *router)
{
    m_router = router;
    m_routing_table.clear();
    m_weight_table.clear();
}

void
RoutingUnit_d::addRoute(const NetDest& routing_table_entry)
{
    m_routing_table.push_back(routing_table_entry);
}

void
RoutingUnit_d::addWeight(int link_weight)
{
    m_weight_table.push_back(link_weight);
}

void
RoutingUnit_d::RC_stage(flit_d *t_flit, InputUnit_d *in_unit, int invc)
{
    /*
       Customize routing
       Written by kagami
    */
    int outport = routeCompute(t_flit, in_unit, invc);
    //int outport = routeCompute(t_flit);

    in_unit->updateRoute(invc, outport, m_router->curCycle());
    t_flit->advance_stage(VA_, m_router->curCycle());
    m_router->vcarb_req();
}

int
RoutingUnit_d::routeCompute(flit_d *t_flit, InputUnit_d *in_unit, int invc)
// Modified by kagami
// RoutingUnit_d::routeCompute(flit_d *t_flit)
{
    MsgPtr msg_ptr = t_flit->get_msg_ptr();
    NetworkMessage* net_msg_ptr = safe_cast<NetworkMessage *>(msg_ptr.get());
    NetDest msg_destination = net_msg_ptr->getInternalDestination();

    int output_link = -1;
    int min_weight = INFINITE_;

    /*
       Customize routing
       Written by kagami
    */
    if (m_router->use_customize_routing()) {
      int dest_router = t_flit->get_dest_router();
      int next_id = -1;
      int next_vc = -1;
      bool is_router = false;

      if (dest_router != m_router->get_id()) {
        is_router = true;
        int vnet = t_flit->get_vnet();
        int vc_offset = invc - vnet * m_router->get_vc_per_vnet();

        const RoutingTableEntry *next_entry = m_router->find_next_router(dest_router, vc_offset);

        if (next_entry == NULL) {
          fatal("Fatal Error:: Routing table is wrong.");
          exit(0);
        }

        next_id = next_entry->get_next_router();
        next_vc = next_entry->get_outvc();
        DPRINTF(RubyNetwork,
            "[Router %d] Routing Computation (dest %d next %d): %lld\n",
            m_router->get_id(), dest_router, next_id, m_router->curCycle());
      } else {
        is_router = false;
        std::vector<NodeID> dest_nodes = msg_destination.getAllDest();
        next_id = dest_nodes[0];
        next_vc = 0;
      }

      output_link = m_router->find_next_port(next_id, is_router);
      in_unit->set_next_vc(invc, next_vc);

    } else {
      for (int link = 0; link < m_routing_table.size(); link++) {
        if (msg_destination.intersectionIsNotEmpty(m_routing_table[link])) {
          if (m_weight_table[link] >= min_weight)
            continue;
          output_link = link;
          min_weight = m_weight_table[link];
        }
      }
    }

    if (output_link == -1) {
        fatal("Fatal Error:: No Route exists from this Router.");
        exit(0);
    }

    return output_link;
}
