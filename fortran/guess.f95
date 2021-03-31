integer(4) function get_max()
   implicit none
   character(32) :: smax

   if (command_argument_count() /= 1) then
      get_max = 100
   else
      call get_command_argument(1, smax)
      read (smax(:), *) get_max
   end if
end function get_max

program guessing
implicit none
   integer :: guess, guesses
   integer(4) :: secret
   character(32) :: input
   real :: s

   call random_init(.false., .true.)
   call random_number(s)

   secret = modulo(floor(s*huge(secret)), get_max())
123 print *, secret

end program guessing
