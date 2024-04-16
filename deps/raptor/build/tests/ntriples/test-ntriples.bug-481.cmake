
EXECUTE_PROCESS(
	COMMAND /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/utils/rapper -q -i nquads -o nquads file:/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/ntriples/bug-481.nq http://librdf.org/raptor/tests/bug-481.nq
	TIMEOUT 10
	OUTPUT_FILE bug-481.res.raw
	ERROR_QUIET
)

# http://public.kitware.com/Bug/view.php?id=13007

CONFIGURE_FILE(
	/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/tests/ntriples/bug-481.res.raw
	bug-481.res
	@ONLY
	NEWLINE_STYLE UNIX
)
