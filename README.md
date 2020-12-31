# George

Georges is a toy project implementing a very simple CPU ray marcher. It was just an opportunity for me to:
- Remember how to program in C++. Last time I had to write C++ was many years ago in an internship, and I kind of missed it.
- Learn more about SIMD intrisics. It would have been much smarter to just use one of the many existing optimized vector libraries, but doing it by hand was a good learning experience. I actually expected  the compiler to take care of this, but this didn't work out as well as expected, so I had to learn how to do it myself.
- Finally implement a ray marcher after spending too many years passively reading from [Inigo Quilez](https://www.iquilezles.org/)'s website.

I'm not 100% happy with the result and it's still very much a WIP, but I wanted to send the code to a friend, and GitHub is a good way to share code.

What I should have done better:
- Writing a Makefile. I used VSCode and the C++ plugin for development so I didn't have to worry about building, but it sucks for other people (meaning, me in a month) who might want to run this.
- Unit tests. I spent far too much time troubleshooting too many small issues in the optimized SIMD implementation. Also this would have been an opportunity to learn about C++ unit testing.
-  I wish I could have spent more time profiling this, but at the time of writing, valgrind is broken on MacOS Big Sur, so memory profiling will have to wait. I was able to do some profiling with Instruments, but it's not a great tool...
- Benchmarking. A lot of the optimizations were not rigorously benchmarked. Now a raymarcher has the advantage of almost being a benchmarking tool by itself since it basically runs the same code many many times. Still, this could have been another learning opportunity. 
- Documentations. Comments. Cleaner code. It's a toy project and I only focused on features and speed, so the rest is quite bad.