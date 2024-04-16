
EXECUTE_PROCESS(
	COMMAND /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/utils/rapper -q -i trig -o nquads /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/trig/example2.trig http://example.librdf.org/example2.trig
	TIMEOUT 10
	OUTPUT_FILE example2.res.raw
	ERROR_QUIET
)

# http://public.kitware.com/Bug/view.php?id=13007

CONFIGURE_FILE(
	/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/tests/trig/example2.res.raw
	example2.res
	@ONLY
	NEWLINE_STYLE UNIX
)
