function compare_two_files($filename)
{

    # Compile
    gcc -c .\cl_utils.c; gcc -c .\main.c
    gcc .\cl_utils.o .\main.o -o test

    # Read input file
    $bin_file_name = $filename + ".bin"
    $bin_file_path = Join-Path .\test\test_input\ $bin_file_name

    # Run
    .\test.exe  $bin_file_path > actual.txt


    # Read expect file
    $expect_file_name = $filename + ".txt"
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
$input_file_names = @("hello_arm", "print_loop", "print_hex")

$input_file_names | ForEach-Object{
    compare_two_files $_
}
