package main

import (
	"os"

	"github.com/fantasticfears/wdbib/commands"
	"gopkg.in/urfave/cli.v2" // imports as package "cli"
)

func main() {
	commands := []*cli.Command{&commands.Add, &commands.List, &commands.Update, &commands.Search, &commands.Version}
	app := &cli.App{
		Name:        "wdbib",
		Description: "a program manages bibliographical library based on Wikidata.",
		HideHelp:    true,
		HideVersion: true,
		Commands:    commands,
	}

	app.Run(os.Args)
}
