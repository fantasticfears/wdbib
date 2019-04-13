package internals

import (
	"strings"
	"testing"

	"github.com/fantasticfears/wdbib/testhelper"
)

func TestUserAgent(t *testing.T) {
	testhelper.Assert(t, strings.Contains(UserAgent, "wdbib"), "can extract the user agent")
}
