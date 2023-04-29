find_path(WX_INCLUDE wx/wx.h PATHS ${CMAKE_CURRENT_LIST_DIR}/include)
find_program(WXRC_BIN wxrc PATHS ${CMAKE_CURRENT_LIST_DIR}/bin)
  
set(_suffix vs11)
	
if(${AZI_STATIC_LIBS})
  set( _libtype lib )
else(${AZI_STATIC_LIBS})
	SET( _libtype dll )
endif(${AZI_STATIC_LIBS})

set( _bitinfo x64_ )
	
find_path(WX_LIB_RELEASE wxbase29u.lib PATHS ${CMAKE_CURRENT_LIST_DIR}/lib/vc_${_bitinfo}${_libtype}${_suffix})
find_path(WX_LIB_DEBUG wxbase29ud.lib PATHS ${CMAKE_CURRENT_LIST_DIR}/lib/vc_${_bitinfo}${_libtype}${_suffix})
    
include_directories(${WX_INCLUDE})
	
if(WX_LIB_RELEASE)
  include_directories(${WX_LIB_RELEASE}/mswu)
else( WX_LIB_RELEASE )
  include_directories(${WX_LIB_DEBUG}/mswud)
endif(WX_LIB_RELEASE)
	
add_definitions(-D_UNICODE -DUNICODE)
	
IF(${AZI_STATIC_LIBS})
  set(Wx_LIBRARIES optimized ${WX_LIB_RELEASE}/wxbase29u.lib optimized ${WX_LIB_RELEASE}/wxbase29u_net.lib optimized ${WX_LIB_RELEASE}/wxbase29u_xml.lib optimized ${WX_LIB_RELEASE}/wxexpat.lib optimized ${WX_LIB_RELEASE}/wxjpeg.lib optimized ${WX_LIB_RELEASE}/wxmsw29u_adv.lib optimized ${WX_LIB_RELEASE}/wxmsw29u_aui.lib optimized ${WX_LIB_RELEASE}/wxmsw29u_core.lib optimized ${WX_LIB_RELEASE}/wxmsw29u_html.lib optimized ${WX_LIB_RELEASE}/wxmsw29u_media.lib optimized ${WX_LIB_RELEASE}/wxmsw29u_qa.lib optimized ${WX_LIB_RELEASE}/wxmsw29u_richtext.lib optimized ${WX_LIB_RELEASE}/wxmsw29u_xrc.lib optimized ${WX_LIB_RELEASE}/wxpng.lib optimized ${WX_LIB_RELEASE}/wxregexu.lib optimized ${WX_LIB_RELEASE}/wxtiff.lib optimized ${WX_LIB_RELEASE}/wxzlib.lib debug ${WX_LIB_DEBUG}/wxbase29ud.lib debug ${WX_LIB_DEBUG}/wxbase29ud_net.lib debug ${WX_LIB_DEBUG}/wxbase29ud_xml.lib debug ${WX_LIB_DEBUG}/wxexpatd.lib debug ${WX_LIB_DEBUG}/wxjpegd.lib debug ${WX_LIB_DEBUG}/wxmsw29ud_adv.lib debug ${WX_LIB_DEBUG}/wxmsw29ud_aui.lib debug ${WX_LIB_DEBUG}/wxmsw29ud_core.lib debug ${WX_LIB_DEBUG}/wxmsw29ud_html.lib debug ${WX_LIB_DEBUG}/wxmsw29ud_media.lib debug ${WX_LIB_DEBUG}/wxmsw29ud_qa.lib debug ${WX_LIB_DEBUG}/wxmsw29ud_richtext.lib debug ${WX_LIB_DEBUG}/wxmsw29ud_xrc.lib debug ${WX_LIB_DEBUG}/wxpngd.lib debug ${WX_LIB_DEBUG}/wxregexud.lib debug ${WX_LIB_DEBUG}/wxtiffd.lib debug ${WX_LIB_DEBUG}/wxzlibd.lib Ws2_32.lib comctl32.lib Rpcrt4.lib)
ELSE(${AZI_STATIC_LIBS})
  add_definitions( -DWXUSINGDLL=1 )
  set(Wx_LIBRARIES optimized ${WX_LIB_RELEASE}/wxbase29u.lib optimized ${WX_LIB_RELEASE}/wxbase29u_net.lib optimized ${WX_LIB_RELEASE}/wxbase29u_xml.lib optimized ${WX_LIB_RELEASE}/wxmsw29u_adv.lib optimized ${WX_LIB_RELEASE}/wxmsw29u_aui.lib optimized ${WX_LIB_RELEASE}/wxmsw29u_core.lib optimized ${WX_LIB_RELEASE}/wxmsw29u_html.lib optimized ${WX_LIB_RELEASE}/wxmsw29u_media.lib optimized ${WX_LIB_RELEASE}/wxmsw29u_qa.lib optimized ${WX_LIB_RELEASE}/wxmsw29u_richtext.lib optimized ${WX_LIB_RELEASE}/wxmsw29u_xrc.lib debug ${WX_LIB_DEBUG}/wxbase29ud.lib debug ${WX_LIB_DEBUG}/wxbase29ud_net.lib debug ${WX_LIB_DEBUG}/wxbase29ud_xml.lib debug ${WX_LIB_DEBUG}/wxmsw29ud_adv.lib debug ${WX_LIB_DEBUG}/wxmsw29ud_aui.lib debug ${WX_LIB_DEBUG}/wxmsw29ud_core.lib debug ${WX_LIB_DEBUG}/wxmsw29ud_html.lib debug ${WX_LIB_DEBUG}/wxmsw29ud_media.lib debug ${WX_LIB_DEBUG}/wxmsw29ud_qa.lib debug ${WX_LIB_DEBUG}/wxmsw29ud_richtext.lib debug ${WX_LIB_DEBUG}/wxmsw29ud_xrc.lib)
ENDIF(${AZI_STATIC_LIBS})
  
