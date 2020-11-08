add_executable("WimpyViewer" "WimpyViewer.cpp")
target_link_libraries("WimpyViewer" ngf)
install(TARGETS "WimpyViewer" DESTINATION "bin/demos")
