$ErrorActionPreference = "Stop"

# test.ps1 - Test script for SimpCalc
# Authors: Keith Ayeras, Member B, Member C

Write-Host "Compiling..."
gcc -Wall -o simpcalc.exe main.c scanner.c parser.c
if ($LASTEXITCODE -ne 0) {
    Write-Host "Compilation failed!" -ForegroundColor Red
    exit 1
}

$tempDir = "test_run"
if (Test-Path $tempDir) {
    Remove-Item -Recurse -Force $tempDir
}
New-Item -ItemType Directory -Path $tempDir | Out-Null

Write-Host "Copying simpcalc.exe and inputs..."
Copy-Item "simpcalc.exe" -Destination $tempDir
Copy-Item "CORRECT_SAMPLES\sample_input_*.txt" -Destination $tempDir

Set-Location $tempDir
Write-Host "Running simpcalc..."
.\simpcalc.exe

Set-Location ..

$failedCount = 0
$passedCount = 0

for ($i = 1; $i -le 9; $i++) {
    $scanOriginal = "CORRECT_SAMPLES\sample_output_scan_$i.txt"
    $scanGenerated = "$tempDir\sample_output_scan_$i.txt"
    
    $parseOriginal = "CORRECT_SAMPLES\sample_output_parse_$i.txt"
    $parseGenerated = "$tempDir\sample_output_parse_$i.txt"
    
    # Test Scanner
    if (Test-Path $scanOriginal) {
        $orig = Get-Content $scanOriginal
        $gen = Get-Content $scanGenerated
        if ($null -eq $orig) { $orig = @() }
        if ($null -eq $gen) { $gen = @() }
        $diffScan = (Compare-Object $orig $gen -CaseSensitive)
        if ($diffScan) {
            Write-Host "FAIL: Scanner Output $i differs" -ForegroundColor Red
            $failedCount++
        } else {
            Write-Host "PASS: Scanner Output $i" -ForegroundColor Green
            $passedCount++
        }
    }
    
    # Test Parser
    if (Test-Path $parseOriginal) {
        $orig = Get-Content $parseOriginal
        $gen = Get-Content $parseGenerated
        if ($null -eq $orig) { $orig = @() }
        if ($null -eq $gen) { $gen = @() }
        $diffParse = (Compare-Object $orig $gen -CaseSensitive)
        if ($diffParse) {
            Write-Host "FAIL: Parser Output $i differs" -ForegroundColor Red
            $failedCount++
        } else {
            Write-Host "PASS: Parser Output $i" -ForegroundColor Green
            $passedCount++
        }
    }
}

Write-Host ""
Write-Host "Tests Complete: $passedCount Passed, $failedCount Failed"
if ($failedCount -eq 0) {
    Write-Host "ALL TESTS PASSED!" -ForegroundColor Green
} else {
    Write-Host "SOME TESTS FAILED." -ForegroundColor Red
}
