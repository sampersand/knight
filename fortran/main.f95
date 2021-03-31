program calling_func

   real :: a
   a = area_of_circle(2.0) 
   
   Print *, "The area of a circle with radius 2.0 is"
   Print *, a
   
end program calling_func


! this function computes the area of a circle with radius r  
function area_of_circle (r) ! result foo

! function result     
implicit none      

   ! dummy arguments        
   real :: area_of_circle   
   
   ! local variables 
   real :: r     
   real :: pi
   
   pi = 4 * atan (1.0)     
   if (.true.) then
      return
   endif
   area_of_circle = pi * r**2  
   !foo = 3
   
end function area_of_circle

! 10 program testing
! implicit none

!    integer, dimension (:), allocatable :: a
!    integer :: b(5), i

!    data b /1, 4, 9, 16, 25/

!    allocate (a(5))
!    do i = 1, 5
!       a(i) = (6 - i) ** 2
!    end do

!    do i = 1, 5
!       Print*, i, a(i), b(i)
!    end do

!    deallocate (a)

!    99 Print "(1i0)", 512345913

!    print 100
!    101 format (7x,'Name:', 7x, 'Id:', 1x, 'Weight:')
!    ! data a /1,2,3/
! ! integer n, nfact
! ! character(len=40) :: itos

! ! nfact=1
! ! n = 1
! ! do while (n < 10)
! !    nfact = nfact * n  
! !    ! printing the value of n and its factorial
! !    Print*,  n, " ", nfact   
! !    n = n + 1

! ! ! 100 goto 100
! ! end do


! ! write(itos,*) huge(n)
! ! print*, adjustr("hello" // itos)

!    ! integer :: a, b

!    ! a = 9
!    ! if (b < 3) then
!    ! endif
!    ! while (.false.) 
!    ! endwhile

! ! ! This simple program adds two numbers
! !       implicit none

! ! ! Type declarations
! !       real :: a, b, result
! !       integer(kind = 1) :: i, n, val
! !       character(len=1) :: msg
! !       logical :: x
! !       x = a /= b
! !       msg = "hello world"

! !       print *, x !msg
! ! !      .true.
! ! !
! ! !! Executable statements
! ! !      a = 12.0
! ! !      b = 15.0
! ! !      result = a + b
! ! !      print *, 'The total is ', result
! ! !
! ! !      i = 0
! ! !      n = 0
! ! !      !while (i < 10)
! ! !      !end while
! ! !      print *, huge(i)

! !
! end program testing
