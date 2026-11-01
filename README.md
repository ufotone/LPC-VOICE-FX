# LPC VOICE FX

[日本語](README.md) | [English](README_EN.md) | [ダウンロード](https://github.com/ufotone/LPC-VOICE-FX/releases/latest)

drumlogue用の音声エフェクトです。入力した声をロボット声に変換します。

## インストール

1. [Release](https://github.com/ufotone/LPC-VOICE-FX/releases/latest)からZIPをダウンロードして展開します。
2. drumlogueの電源を切り、RECを押しながら起動してUSBマスストレージ・モードにします。
3. `lpc_voice_fx.drmlgunit`を本体の`Units/MasterFXs/`にコピーします。
4. 安全に取り外し、PLAY/STOPでUSBモードを終了します。
5. Master FXで**LPC VOICE FX**を選択します。

マイクなどの音声を使うときは、AUDIO INに接続し、`SHIFT`＋`BD`のAUDIO IN設定で`ROUTE`を`SIDECH`、エフェクトの`INPUT`を`SIDECHAIN`にします。

## パラメーター

| ページ | 名前 | 範囲 | 初期値 | 説明 |
|---|---|---|---|---|
| 1 | INPUT | SIDECHAIN / MAIN | SIDECHAIN | 加工する音の入力元を選択。SIDECHAINは外部入力、MAINはメインのミックス全体。 |
| 1 | IN GAIN | 0〜＋60dB | ＋18dB | 入力音の増幅量。 |
| 1 | PITCH | −24〜＋36半音 | 0 | ロボット声の音程。0は約130Hz。 |
| 1 | MIX | 0〜100% | 100% | SIDECHAINでは加工した声を加える量。MAINでは原音と加工音の割合。 |
| 2 | LEVEL | 0〜100% | 80% | リバーブを含む加工した声の音量。 |
| 2 | LOW CUT | 20〜1000Hz | 120Hz | 加工した声の低音をカット。数値を上げると、より高い周波数まで抑えます。 |
| 2 | REV MIX | 0〜100% | 25% | 加工した声にかけるリバーブの深さ。0%でオフ。 |
| 2 | REV TIME | 200〜4000ms | 1500ms | リバーブの余韻の長さ。 |

SIDECHAINではドラムなどのメイン音に加工した声を加えます。LOW CUTとリバーブは加工した声にかかります。MAINではメインの左右をまとめて加工します。
