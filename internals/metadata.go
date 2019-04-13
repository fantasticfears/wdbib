package internals

import (
	"fmt"
	"go/build"
)

var VersionStr = "0.0.1"

var UserAgent = fmt.Sprintf("wdbib/%s %s-%s %s",
	VersionStr, build.Default.GOOS, build.Default.GOARCH, build.Default.ReleaseTags[len(build.Default.ReleaseTags)-1])
