// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 156.

// Package geometry defines simple types for plane geometry.
//!+point
package main
//package geometry
import(
 "math"
 "fmt"
 "os"
 "math/rand"
 "strconv"
)

type Point struct{ x, y float64 }

// traditional function
func Distance(p, q Point) float64 {
	return math.Hypot(q.x-p.x, q.y-p.y)
}

// same thing, but as a method of the Point type
func (p Point) Distance(q Point) float64 {
	return math.Hypot(q.x-p.x, q.y-p.y)
}

//!-point

func orientation(p, q, r Point) int {
	val := (q.y-p.y)*(r.x-q.x) - (q.x-p.x)*(r.y-q.y)
	if val == 0 {
		return 0 //colinear
	}
	if val > 0 {
		return 1 //clockwise
	}
	return 2 //counterclockwise
}

func (p Point) X() float64 {
	return p.x
}

func (p Point) Y() float64 {
	return p.y
}

//!+path


// A Path is a journey connecting the points with straight lines.
type Path []Point

// Distance returns the distance traveled along the path.
func (path Path) Distance() float64 {
	sum := 0.0
	for i := range path {
		if i > 0 {
			sum += path[i-1].Distance(path[i])
		}
	}
	return sum
}

//!-path

func createLines(array Path) []Path {
	var lines []Path = make([]Path, len(array)-1)
	for i := 0; i < len(array)-1; i++ { //create lines
		lines[i] = Path{array[i], array[i+1]}
	}
	return lines
}


func main() {
	arg:=os.Args[1]
        sides, err := strconv.Atoi(arg)
        if err != nil {
                fmt.Println(err)
		fmt.Println("parametro debe de ser un numero valido")
		return
        }
	if sides<3{
		fmt.Println("numero de lados necesitan ser mayores a 2")
		return
	}
	arr := make(Path, sides+1)
	//generate points(check for crosses with at least 4 points, meaning at least 2 lines)
	collided := true
	for collided {
		collided = false
		for i := 0; i < sides; i++ {
			arr[i] = Point{rand.Float64()*200 - 100, rand.Float64()*200 - 100} //random points
		}
		arr[sides] = arr[0]
		if sides >= 4 {
			lines := createLines(arr)
			for i, v := range lines {
				for _, v2 := range lines[i+1:] {
					//p1=v[0],q1=v[1],p2=v2[0],q2=v2[1]
					if v[0] == v2[1] || v2[0] == v[1] {
						continue
					}
					if (orientation(v[0], v[1], v2[0]) != orientation(v[0], v[1], v2[1])) != (orientation(v2[0], v2[1], v[1]) != orientation(v2[0], v2[1], v[1])) {
						collided = true
						//fmt.Println(v, v2)
						//fmt.Println(lines)
						//temp := v[0]
						//v[0] = v2[1]
						//v2[1] = temp
						//fmt.Println(lines)
						break
					}
				}
				if collided == true {
					break
				}
			}
		}

	}
	fmt.Println("-Figures Vertices")
	for _, v := range arr[:len(arr)-1] {
		fmt.Println("(", v.x, ",", v.y, ")")
	}
	fmt.Println("-Figures Perimeter")
	for i := range arr[1:] {
		fmt.Print(arr[i].Distance(arr[i+1]))
		if i < len(arr[1:])-1 {
			fmt.Print(" + ")
		}
	}
	fmt.Println(" =", arr.Distance())

}
