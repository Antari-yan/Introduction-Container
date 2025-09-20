package main

import (
  "fmt"
  "runtime"
)

func main() {
  fmt.Println("Golang Container. Running on: ", runtime.GOOS , "-", runtime.GOARCH)
}