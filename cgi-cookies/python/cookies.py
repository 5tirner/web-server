 #!/usr/bin/python
#
#  py-cgi-cookies
#
#  This  program is free software: you can redistribute it and/or modify  it
#  under  the  terms of the GNU General Public License as published  by  the
#  Free  Software  Foundation, either version 3 of the License, or (at  your
#  option) any later version.
#
#  This  program  is  distributed in the hope that it will  be  useful,  but
#  WITHOUT   ANY   WARRANTY;   without  even   the   implied   warranty   of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
#  Public License for more details.
#
#  You  should have received a copy of the GNU General Public License  along
#  with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#  10 Apr 20   0.1   - Initial version - MT
#
import os, time

VERSION = 0.1

def get_cookie(_match): 
  # Returns the value from the matching cookie or '' if not defined.
  if 'HTTP_COOKIE' in os.environ:
    cookies = os.environ['HTTP_COOKIE']
    cookies = cookies.split('; ')

    for cookie in cookies:
      (_name, _value) = cookie.split('=')
      if (_match.lower() == _name.lower()):
        return _value
  return('')

def val(_string): 
  # Returns the integer value of a string or zero if not numeric.
  try:
    return int(_string)
  except:
    return 0

print ('Content-Type: text/html;charset=utf-8') 
if not get_cookie('first'): # Don't redefine value if aleady defined.
  print ('Set-Cookie: First=' + time.asctime(time.localtime()))
print ('Set-Cookie: Last=' + time.asctime(time.localtime()))
print ('Set-Cookie: Count=' + str(val(get_cookie('Count')) + 1)) # Increment counter.
print ('')# End of header.

print ('<html><body><table border="0">')
if get_cookie('first'): # Only dispay value if cookie is defined.
  print ('<tr><td>Time First Visit</td><td>:</td><td>' + get_cookie('first') + '</tr>')
if get_cookie('last'): # Only dispay value if cookie is defined.
  print ('<tr><td>Time Last Visit</td><td>:</td><td>' + get_cookie('last') + '</tr>')
print ('<tr><td>Counter</td><td>:</td><td>' + str(val(get_cookie('count'))) + '</tr>')
print ('</table></body></html>')
