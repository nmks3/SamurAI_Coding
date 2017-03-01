# SamurAI_Coding

## 概要
情報処理学会主催の「SamurAI Coding」用のプログラム作成を目的としている。  
プログラムにはCNNを用いた教師あり学習をする。
実行手順など以下に示す。

## フォルダ説明
arukuka_mztmr_nggts : 昨年上位者プログラム  
piyo : 昨年上位者プログラム  
sinryakudegeso : 昨年上位者プログラム  
manager : ゲーム実行、各種プログラムの実行ファイル  
Python : python言語によるソースコード  
pythonplayer : 2015-16大会用のプログラム  
traindata : 学習に必要なデータ  

## 実行手順
### 環境構築
#### Homebrew
pyenvを用いるのでまずはHomebrewをインストールします。
[Homebrew](https://brew.sh/index_ja.html)にあるインストール手順に従い、コマンドをコピーし、実行します。    
`$ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"` (※2017/3/1現在)  

#### pyenv
続いて、pyenvをインストールします。インストール前にhomebrewの環境を最新のものにしておきます。  
`$ brew update && brew upgrade`  
pyenvインストール  
`$ brew install pyenv`  
環境変数を追記  
`$ echo 'export PYENV_ROOT="${HOME}/.pyenv"' >> ~/.bash_profile`  
`$ echo 'export PATH="${PYENV_ROOT}/bin:$PATH"' >> ~/.bash_profile`
`$ echo 'eval "$(pyenv init -)"' >> ~/.bash_profile`  
`$ source ~/.bash_profile`  


####
### 教師データ収集
まず、学習に必要な教師データを収集します。教師データは「SamurAI Coding 2015-16」の上位入賞者同士を対戦させたログデータです。  
