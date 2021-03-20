program helloWorld
implicit none

integer n, nfact
character(len=40) :: itos

nfact=1
n = 1
do while (n < 10)
   nfact = nfact * n  
   ! printing the value of n and its factorial
   Print*,  n, " ", nfact   
   n = n + 1

100 goto 100
end do


write(itos,*) huge(n)
print*, "hello" // itos

   ! integer :: a, b

   ! a = 9
   ! if (b < 3) then
   ! endif
   ! while (.false.) 
   ! endwhile

! ! This simple program adds two numbers
!       implicit none

! ! Type declarations
!       real :: a, b, result
!       integer(kind = 1) :: i, n, val
!       character(len=1) :: msg
!       logical :: x
!       x = a /= b
!       msg = "hello world"

!       print *, x !msg
! !      .true.
! !
! !! Executable statements
! !      a = 12.0
! !      b = 15.0
! !      result = a + b
! !      print *, 'The total is ', result
! !
! !      i = 0
! !      n = 0
! !      !while (i < 10)
! !      !end while
! !      print *, huge(i)
!
end program helloWorld
