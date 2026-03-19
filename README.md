<h1 align="center">
    <picture>
      <img height="300px" style="margin: 0; padding: 0" src="assets/logo-512x512.png">
    </picture>
</h1>

Coda - A personal database for music collectors
=========

## Info

Coda is a command line application built in C. Under the hood, it uses the SQLite library for information management and searching. Coda is meant to be used as a way to track and browse a personal music collection such as CD's or records.

## Background
This project started because my CD collection started to get unruly and I would find new CD's and forget which ones I already owned. 

In music theory, a coda is the closing section that brings a musical composition together. Coda, the project, is named such because it serves as the final piece that ties a music collection together; an easy to use organizational tool (it also is just a really cool sounding name).

## Status

Coda is still in the early stages of development. This is a personal project of mine that I'm working on in between other commitments so updates may be intermittent. 

## Building

Coda uses a lightweight build tool called [Tiny](https://github.com/JHeflinger/tiny) created by my good friend [Jason](https://jasonheflinger.com/). Building Coda is as simple as cloning the repository and running ```./tiny```.

## How To Use

Once built, you can use the ```run.sh``` script to interface with the database. I recommend creating an alias for this for ease of use, for the rest of the documentation I will be referring to the command as ```coda```.

### Commands

| Command | Arguments | Description | Status |
| ------- | --------- | ----------- | ------ |
| ```coda init``` | ```<database name>.db``` | This is the first thing you should run when getting started. Here you will name your database whatever you like. The database will be created if one doesn't exist already. You can also use init to switch between different databases if you wish to have multiple collections. | Working |
| ```coda add``` | ```[-a album] [-A artist] [-d date]``` | This is how you add new items to your collection. You can run this command with the flags in any order you like. Album and Artist name is a required argument but anything else is optional. Currently you can only add whole albums. | WIP |
| ```coda search``` | ```[-i id] [-a album] [-A artist]``` | This is how you can search for items in your collection. You can choose to use one flag or multiple flags. The function will return a list sorted by search criteria of all matching albums. This is the main way to look up the unique ID of items in your collection in order to manipulate them with other commands | WIP |
| ```coda edit``` | ```<id> [-a album], [-A artist], [-d date]``` | This is how you can update an existing entry with new or missing information. You need the unique ID of the album to edit. This command accepts singular or multiple flags. | WIP |
| ```coda list``` | ```[-a, --album], [-A, --artist], [-d, --date]```| This command prints out a sorted list of your entire collection. A flag is optional if you'd like to sort by a specific category. | WIP |
| ```coda remove``` | ```<id>``` | Remove an item from collection by ID number | WIP |

### Configuration

```.codaconf``` is where optional settings live

| Precursor | Value | Description |
| --------- | ----- | ----------- |
| CURRENT_DB | ```<database_name>.db``` | This is the database that you are interfacing with, changing this value is how you can switch between different collections. Using ```code init``` automatically populates this configuration. |

## Future Plans

There are a handful of features which I am not including in the initial release which I'd like to eventually support:
- Individual Tracks Collection: A way to track individual tracks which automatically connects to your album collection, for those who are especially meticulous.
- More Metadata!: More optional flags to get even more detailed (e.g. genres, length, record label)
- Personal Rating System: Rank your collection however you see fit (I really like the way the restaurant review app *Beli* handles this).
- Mobile/web integration: I personally really want to include a music collection tab on my [personal blog](https://cameron-lee.com/).
