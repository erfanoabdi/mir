add_library(mirudev OBJECT
  udev_wrapper.cpp
)

set_target_properties(mirudev PROPERTIES LINK_FLAGS ${UDEV_LDFLAGS})
set_target_properties(mirudev PROPERTIES INTERFACE_LINK_LIBRARIES ${UDEV_LIBRARIES})

list(APPEND MIR_PLATFORM_REFERENCES ${UDEV_LDFLAGS} ${UDEV_LIBRARIES})
set(MIR_PLATFORM_REFERENCES ${MIR_PLATFORM_REFERENCES} PARENT_SCOPE)
