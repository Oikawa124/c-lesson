# Compile and run
gcc -c .\cl_utils.c; gcc -c .\main.c
gcc .\cl_utils.o .\main.o -o test

.\test.exe hello_arm.bin > actual.txt

# Read files
$expect = Get-Content .\test\test_expect\hello_arm.txt
$actual = Get-Content actual.txt


# Compare strings
0..($expect.Count-1) | ForEach-Object{
    $isEqual = $expect[$_] -eq $actual[$_]

    if (-Not($isEqual))
    {
        Write-Host -NONewline  $expect[$_]' | '$actual[$_].Substring($actual[$_].Length -12, 12)
        return # Not at "break"
    }
}


# Delete file
Remove-Item *.o
Remove-Item test.exe
Remove-Item actual.txt


# print example >> 0x00010038  00 10 1f 11  |  00 10 1f 10
