package main

import (
  "fmt"
  "runtime"
)

func main() {
  fmt.Println("Go Go Golang Container. Running on: ", runtime.GOOS , "-", runtime.GOARCH)
}