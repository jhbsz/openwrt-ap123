define Profile/AP123
	NAME:=AP123
	PACKAGES:=
endef

define Profile/AP123/Description
	Package set optimized for AP123
endef
$(eval $(call Profile,AP123))