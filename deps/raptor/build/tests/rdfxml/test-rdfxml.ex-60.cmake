
EXECUTE_PROCESS(
	COMMAND /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/utils/rapper -q -i ntriples -o rdfxml /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/rdfxml/ex-60.nt -
	TIMEOUT 10
	OUTPUT_FILE ex-60-rdfxml.rdf.raw
	ERROR_QUIET
)

# http://public.kitware.com/Bug/view.php?id=13007

CONFIGURE_FILE(
	/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/tests/rdfxml/ex-60-rdfxml.rdf.raw
	ex-60-rdfxml.rdf
	@ONLY
	NEWLINE_STYLE UNIX
)
