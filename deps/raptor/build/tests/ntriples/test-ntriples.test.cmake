
EXECUTE_PROCESS(
	COMMAND /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/utils/rapper -q -i ntriples -o ntriples file:/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/ntriples/test.nt http://librdf.org/raptor/tests/test.nt
	TIMEOUT 10
	OUTPUT_FILE test.res.raw
	ERROR_QUIET
)

# http://public.kitware.com/Bug/view.php?id=13007

CONFIGURE_FILE(
	/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/tests/ntriples/test.res.raw
	test.res
	@ONLY
	NEWLINE_STYLE UNIX
)
