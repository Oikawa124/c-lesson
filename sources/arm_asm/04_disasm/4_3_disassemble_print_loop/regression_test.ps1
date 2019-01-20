function compare_two_files($filename)
{
    # Compile and run
    gcc -c .\cl_utils.c; gcc -c .\main.c
    gcc .\cl_utils.o .\main.o -o test

    $bin_file = $filename + ".bin"

    .\test.exe  $bin_file > actual.txt

    # Read files
    $expect_file_name = $file_name + ".txt"
    $expect_file_path = Join-Path .\test\test_expect\ $expect_file_name

    $expect = Get-Content $expect_file_path
    $actual = Get-Content actual.txt


    # Compare strings
    0..($expect.Count - 1) | ForEach-Object{
        $isEqual = $expect[$_] -eq $actual[$_]

        if (-Not($isEqual))
        {
            Write-Host -NONewline  $expect[$_]' | '$actual[$_].Substring($actual[$_].Length - 12, 12)
            return # Not at "break"
        }
    }


    # Delete file
    Remove-Item *.o
    Remove-Item test.exe
    Remove-Item actual.txt

}


# input filenames
$input_file_names = @("hello_arm", "print_loop")

foreach($file_name in $input_file_names){
    compare_two_files $file_name
}
