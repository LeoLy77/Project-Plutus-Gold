# invoke SourceDir generated makefile for mmw_dss.pe674
mmw_dss.pe674: .libraries,mmw_dss.pe674
.libraries,mmw_dss.pe674: package/cfg/mmw_dss_pe674.xdl
	$(MAKE) -f C:\Users\jeong\workspace_v10\mmwave_sdk_18xx_dss/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\jeong\workspace_v10\mmwave_sdk_18xx_dss/src/makefile.libs clean

