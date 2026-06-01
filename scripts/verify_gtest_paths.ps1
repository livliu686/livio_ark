$ErrorActionPreference = 'Stop'

function Require-Success($code, $name) {
    if ($code -ne 0) {
        throw "${name} failed with exit code ${code}"
    }
}

function Configure-And-Check([string[]]$CmakeArgs, [string]$Name, [string]$ExpectedMode) {
  $null = (& cmake @CmakeArgs 2>&1 | Tee-Object -Variable lines)
  $text = ($lines -join "`n")
  if ($LASTEXITCODE -ne 0) {
    throw "${Name} failed with exit code ${LASTEXITCODE}`n${text}"
  }
  if ($text -notmatch [Regex]::Escape($ExpectedMode)) {
    throw "${Name} did not use expected mode '${ExpectedMode}'.`n${text}"
  }
}

$root = Resolve-Path (Join-Path $PSScriptRoot '..')
Set-Location $root

$missingLocal = Join-Path $root 'thirdparty\\__missing_gtest__'

# 1) FetchContent mode (seed source + package files)
$buildFetch = Join-Path $root 'build\\verify-gtest-fetch'
Configure-And-Check @(
    '-S', '.',
    '-B', $buildFetch,
    '-G', 'Ninja',
    '-DLIVIO_ARK_BUILD_TESTS=ON',
    '-DLIVIO_ARK_FETCH_GTEST=ON',
    "-DLIVIO_ARK_GTEST_LOCAL_SOURCE_DIR=$missingLocal"
) 'verify fetchcontent mode' 'GoogleTest source mode: fetchcontent'

$localSrc = Join-Path $buildFetch '_deps/googletest-src'
if (-not (Test-Path (Join-Path $localSrc 'CMakeLists.txt'))) {
    throw "Expected fetched googletest source not found at: $localSrc"
}
$localSrc = (Resolve-Path $localSrc).Path -replace '\\', '/'

# 2) find_package mode (reuse fetched package config dir)
$gtestPkgBuild = Join-Path $root 'build\\verify-gtest-pkg-build'
$gtestPrefix = Join-Path $root 'build\\verify-gtest-pkg-prefix'
cmake -S $localSrc -B $gtestPkgBuild -G Ninja `
  -DCMAKE_BUILD_TYPE=Debug `
  -DINSTALL_GTEST=ON `
  -DBUILD_GMOCK=ON `
  -DBUILD_SHARED_LIBS=OFF | Out-Host
Require-Success $LASTEXITCODE 'build gtest package configure'

cmake --build $gtestPkgBuild | Out-Host
Require-Success $LASTEXITCODE 'build gtest package'

cmake --install $gtestPkgBuild --prefix $gtestPrefix | Out-Host
Require-Success $LASTEXITCODE 'install gtest package'

$buildFind = Join-Path $root 'build\\verify-gtest-find'
$gtestDir = (Join-Path $gtestPrefix 'lib/cmake/GTest') -replace '\\', '/'
Configure-And-Check @(
    '-S', '.',
    '-B', $buildFind,
    '-G', 'Ninja',
    '-DLIVIO_ARK_BUILD_TESTS=ON',
    '-DLIVIO_ARK_FETCH_GTEST=OFF',
    "-DLIVIO_ARK_GTEST_LOCAL_SOURCE_DIR=$missingLocal",
    "-DGTest_DIR=$gtestDir"
) 'verify find_package mode' 'GoogleTest source mode: find_package'

# 3) Local source path mode
$buildLocal = Join-Path $root 'build\\verify-gtest-local'
Configure-And-Check @(
    '-S', '.',
    '-B', $buildLocal,
    '-G', 'Ninja',
    '-DLIVIO_ARK_BUILD_TESTS=ON',
    '-DLIVIO_ARK_FETCH_GTEST=OFF',
    "-DLIVIO_ARK_GTEST_LOCAL_SOURCE_DIR=$localSrc"
) 'verify local mode' 'GoogleTest source mode: local'

Write-Host 'GTest resolution verification passed: local / find_package / fetchcontent'
