The latest stable release of vg can always be found at: [https://github.com/vgteam/vg/releases](https://github.com/vgteam/vg/releases).

### Release schedule
vg aims to make a new release available every six weeks.

The next release, **1.41.0**, is scheduled for **June 6, 2022**.
* [Release Issue](https://github.com/vgteam/vg/issues/3648)
* [Release Milestone](https://github.com/vgteam/vg/milestone/73)
* [Draft Changelog](https://github.com/vgteam/vg/wiki/Draft-Changelog)

### Versioning
vg currently does not implement semantic versioning. We do try
to minimize breaking changes but there are no guarantees in place.

We do try to maintain a semantic-like naming scheme for releases (e.g.
the major release after 1.7.0 is 1.8.0).

### Docker
The vgteam docker register also receives a new docker image when a release is cut.
These live at [quay.io/vgteam/vg](quay.io/vgteam/vg).

### Before a release
1. Check that all unit and integration tests (both Travis and GitLab) have been ran.
2. Make note of any breaking changes (e.g. deprecated functions, new submodules, additional dependencies, etc)
in the release issue.

### Release process
To cut a new release:  
1. [ ] Create a new release [on Github](https://github.com/vgteam/vg/releases/new)
    1. [ ] Look up the next scheduled codename in [`version.cpp`](https://github.com/vgteam/vg/blob/master/src/version.cpp). If we have run to the end of the list of codenames, quickly PR some more to `master`.
    2. [ ] Tag it  **vX.Y.Z**
    3. [ ] Title it **vg X.Y.Z - Codename Here**
    4. [ ] Mark it as a prerelease
    5. [ ] Publish it (which tags the commit and kicks off the Docker build on Quay)
2. [ ] Produce a buildable source tarball, containing code for `vg` and all submodules, and upload it to the release.
    ```
    mkdir source-tarball
    cd source-tarball
    git clone --recursive https://github.com/vgteam/vg.git
    cd vg
    git fetch --tags origin
    LATEST_TAG="$(git describe --tags `git rev-list --tags --max-count=1`)"
    git checkout "${LATEST_TAG}"
    git submodule update --init --recursive
    make include/vg_git_version.hpp
    rm -Rf .git
    find deps -name ".git" -exec rm -Rf "{}" \;
    cd ..
    mv vg "vg-${LATEST_TAG}"
    tar -czf "vg-${LATEST_TAG}.tar.gz" "vg-${LATEST_TAG}"
    rm -Rf "vg-${LATEST_TAG}"
    ```         
3. [ ] Build and upload a static Linux binary (/bin/vg file) for the release  (Mac OS X can't build static binaries.)
    ```
    cd vg
    git remote add upstream https://github.com/vgteam/vg.git || true
    git fetch --tags upstream
    LATEST_TAG="$(git describe --tags `git rev-list --tags --max-count=1`)"
    git checkout "${LATEST_TAG}"
    git submodule sync
    git submodule update --init --recursive
    rm -f lib/libsdsl.a
    sed -i s/march=native/march=nehalem/ deps/sdsl-lite/CMakeLists.txt
    locate libz.a || (sudo apt-get update && make get-deps)
    CXXFLAGS=" -march=nehalem " make -j8 static
    strip -d bin/vg
    bin/vg test
    ```
4. [ ] In the new release:
    1. [ ] Copy-paste the description of the previous release.
    2. [ ] Insert changes from the [draft changelog](https://github.com/vgteam/vg/wiki/Draft-Changelog)
    3. [ ] List any new dependencies
    4. [ ] List any updated, moved, new, or removed submodules
    It may help to consult the log of commits since the last release.
5. [ ] Clear the [draft changelog](https://github.com/vgteam/vg/wiki/Draft-Changelog) for the next release
6. [ ] Create a milestone for the next release [here](https://github.com/vgteam/vg/milestones/new), named **vX.Y+1.Z**, due in 6 weeks.
7. [ ] Create an issue **Release vg vX.Y+1.Z** for the next release [on the Github issues page](https://github.com/vgteam/vg/issues/new), using the checklist template [from the wiki](https://github.com/vgteam/vg/wiki/Releases).
    1. [ ] Assign it to the milestone.
8. [ ] Update [this wiki page](https://github.com/vgteam/vg/wiki/Releases) with the next release version, date, issue, and milestone.
9. [ ] Check [the Quay build log](https://quay.io/repository/vgteam/vg?tab=builds) to make sure that the Docker build succeeded. If it did not succeed and cannot be made to succeed, you can build and push the container manually:
    ```
    VG_VERSION="vX.Y.Z"
    
    git clone --recursive https://github.com/vgteam/vg.git
    cd vg
    git checkout ${VG_VERSION}
    git submodule sync
    git submodule update --init --recursive
    make include/vg_git_version.hpp
    docker pull mirror.gcr.io/library/ubuntu:20.04
    docker build --target run -t quay.io/vgteam/vg:${VG_VERSION} -f Dockerfile .
    docker build --target test -f Dockerfile .
    
    docker logout quay.io
    docker login quay.io
    docker push quay.io/vgteam/vg:${VG_VERSION}
    docker logout quay.io
    ```
10. [ ] Edit the release and mark it as no longer a pre-release, to make it the latest stable release.
11. [ ] Contact Rose Miyatsu at UCSC by e-mail to promote the release. Send a couple sentences about the milestones reached in the release, a link to the release on Github, and the [vg logo](https://github.com/vgteam/vg/blob/master/doc/figures/vg_logo.png) or any more relevant image available. Include these relevant Twitters: [@BenedictPaten](https://twitter.com/BenedictPaten) [@erikgarrison](https://twitter.com/erikgarrison) [@jeizenga](https://twitter.com/jeizenga) [@jltsiren](https://twitter.com/jltsiren) [@amnovak](https://twitter.com/amnovak) [@JMonlong](https://twitter.com/JMonlong) [@glenn_hickey](https://twitter.com/glenn_hickey) 
12. [ ] Close the previous release issue, and the previous release milestone [in the milestone list](https://github.com/vgteam/vg/milestones).


### References
* [Information on tag finding with Git](https://stackoverflow.com/a/22857288)