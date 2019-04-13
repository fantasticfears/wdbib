package commands

import (
	"fmt"

	"github.com/fantasticfears/wdbib/interals"
	"github.com/fantasticfears/wdbib/storage"
	"gopkg.in/urfave/cli.v2"
)

// Add is the subcommand adding new bib
var Add = cli.Command{
	Name:  "add",
	Usage: "add a citation to the library",
	UsageText: `wdbib add [Q-identifier...]

   Example:
  	 wdbib add -w Q24347512
     wdbib add --wikidata Q24347512`,
	Description: fmt.Sprintf("wdbib adds an uri describing a citation to a file called `%s`"+
		", and downloads the bibliographical item based on Wikidata's QID to a file called `%s`",
		storage.DefaultDataFilename, storage.DefaultCachedDataFilename),
	Action: func(c *cli.Context) error {
		narg := c.NArg()
		if narg <= 0 {
			return cli.Exit("Nothing specified, nothing added.\nTry to add something:\nwdbib add Q24347512", 0)
		}

		for i := 0; i < narg; i++ {
			item := c.Args().Get(i)
			fmt.Println("got", item)
		}
		return nil
	},
}

// List is the subcommand showing current bibliograpical library
var List = cli.Command{
	Name:    "list",
	Aliases: []string{"ls"},
	Usage:   "lists all add a citation to the library",
	UsageText: `wdbib list

   Try:
     wdbib list
     wdbib ls`,
	Description: fmt.Sprintf("wdbib lists the current citations data from `%s` file and lib status.", storage.DefaultDataFilename),
	Action: func(c *cli.Context) error {
		narg := c.NArg()
		if narg <= 0 {
			return cli.Exit("Nothing specified, nothing added.\nTry to add something:\nwdbib add Q24347512", 0)
		}

		for i := 0; i < narg; i++ {
			item := c.Args().Get(i)
			fmt.Println("got", item)
		}
		return nil
	},
}

// Search is the subcommand searching wikicites on Wikidata
var Search = cli.Command{
	Name:  "search",
	Usage: "Search bibliographical items on Wikidata",
	UsageText: `wdbib search <QID/title>

   Try:
     wdbib search Q24347512
     wdbib search \"mammalian taste receptors\"`,
	Description: "wdbib searchs QID or related items on Wikidata for bibliographic information in CLI",
	Action: func(c *cli.Context) error {
		narg := c.NArg()
		if narg <= 0 {
			return cli.Exit("Nothing specified, nothing added.\nTry to add something:\nwdbib add Q24347512", 0)
		}

		for i := 0; i < narg; i++ {
			item := c.Args().Get(i)
			fmt.Println("got", item)
		}
		return nil
	},
}

// Update is the subcommand updating current bibliograpical library
var Update = cli.Command{
	Name:  "update",
	Usage: "update all bibliographical items or selected item",
	UsageText: `wdbib update [QID...]

   Try:
     wdbib update
     wdbib update Q24347512`,
	Description: fmt.Sprintf("wdbib updates items in the `citation` to the latest based on Wikidata,"+
		"and updates `%s`", storage.DefaultCachedDataFilename),
	Action: func(c *cli.Context) error {
		narg := c.NArg()
		if narg <= 0 {
			return cli.Exit("Nothing specified, nothing added.\nTry to add something:\nwdbib add Q24347512", 0)
		}

		for i := 0; i < narg; i++ {
			item := c.Args().Get(i)
			fmt.Println("got", item)
		}
		return nil
	},
}

// Version is the subcommand showing version
var Version = cli.Command{
	Name:     "version",
	Usage:    "Show version of the program.",
	HideHelp: true,
	Action: func(c *cli.Context) error {
		fmt.Println(interals.VersionStr)
		return nil
	},
}
