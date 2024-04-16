
EXECUTE_PROCESS(
	COMMAND /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/utils/rapper -q -i trig -o nquads /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/example3.trig http://example.librdf.org/example3.trig
	TIMEOUT 10
	OUTPUT_FILE example3.res.raw
	ERROR_QUIET
)

# http://public.kitware.com/Bug/view.php?id=13007

CONFIGURE_FILE(
	/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/tests/trig/example3.res.raw
	example3.res
	@ONLY
	NEWLINE_STYLE UNIX
)
