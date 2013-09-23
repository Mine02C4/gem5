from m5.params import *
from m5.SimObject import SimObject

class RoutingTableEntry(SimObject):
    type = 'RoutingTableEntry'
    cxx_header = "mem/ruby/network/garnet/fixed-pipeline/RoutingTableEntry.hh"
    cur_router = Param.Int("Current router");
    dest_router = Param.Int("Destination router");
    invc = Param.Int("Input virtual channel");
    next_router = Param.Int("Next router");
    outvc = Param.Int("Output virtual channel");

    
