echo: 
	@cl.exe /nologo /D "WIN32" /D "_WINDOWS" /D "_MBCS" echo_rest.C echo_util.c /I.\..\..\..\include /c
	@link.exe /nologo echo_rest.obj echo_util.obj /LIBPATH:.\..\..\..\lib axiom.lib axis2_util.lib axis2_engine.lib axis2_parser.lib /OUT:echo_rest.exe

	


