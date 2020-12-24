import cog
import math
import warnings
import time
import sys

sys.path.append(calvos_path)

import calvos.comgen.CAN as nw
import calvos.common.codegen as cg

cg.parse_codegen_spreadsheet(codegen_input)

network = nw.Network_CAN("CAN Network Test")
network.parse_spreadsheet_ods(input_worksheet) 